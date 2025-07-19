#include "thread.h"

/**
 * The web server thread's main function that handles accepting connections and replying to HTTP requests.
 * 
 * @param ctx The thread context.
 * 
 * @return void
 */
void* server__thread(void* ctx) {
    thread_ctx_t* tctx = (thread_ctx_t*)ctx;

    config_t *cfg = tctx->ctx->cfg;

    int ret;

    int sock_fd = tctx->global_sock_fd;

    // Check if we need to setup a separate socket (per socket).
    if (cfg->thread_type == THREAD_TYPE_PER_SOCK) {
        if ((ret = server__socket_setup(&sock_fd, cfg->bind_addr, cfg->bind_port, 1)) != 0) {
            logger__log(cfg, LVL_ERROR, "Failed to create individual socket on thread #%d (%d)", tctx->id, ret);

            free(tctx);

            pthread_exit(NULL);
        }
    }

    logger__log(cfg, LVL_INFO, "Spinning up web server thread #%d (socket FD => %d)...", tctx->id, sock_fd);

    while (1) {
        logger__log(cfg, LVL_TRACE, "Waiting for new connections on thread #%d...", tctx->id);

        // Create buffer for client.
        struct sockaddr_in cl;
        socklen_t cl_len = sizeof(cl);

        // Accept connection.
        int cl_fd = accept(sock_fd, (struct sockaddr*)&cl, &cl_len);

        if (cl_fd < 0) {
            logger__log(cfg, LVL_ERROR, "Failed to accept connection.");

            continue;
        }

        logger__log(cfg, LVL_TRACE, "Accepted new connection!");

        char buffer[4096];
        
        // Read data.
        ssize_t read = recv(cl_fd, buffer, sizeof(buffer) - 1, 0);

        if (read < 0) {
            logger__log(cfg, LVL_ERROR, "Failed to receive data.");

            close(cl_fd);

            continue;
        }

        buffer[read] = '\0';

        logger__log(cfg, LVL_TRACE, "Read %d bytes from request.", read);

        http_request_t req = {0};

        if (utils__http_request_parse(&req, buffer)) {
            error_ctx_t* err = utils__error_ctx();

            logger__log(cfg, LVL_ERROR, "Failed to parse request: %s (%d)", err->msg, err->code);

            close(cl_fd);

            continue;
        }

        logger__log(cfg, LVL_NOTICE, "Method => %s. Path => %s. Version => %s", req.method, req.path, req.version);

        // Create response.
        http_response_t res = {0};
        strcpy(res.version, "HTTP/1.1");

        // Retrieve HTML contents for body and check for error.
        char *res_body = NULL;

        if (fs__web_get_html(req.path, cfg->public_dir, &res_body)) {
            error_ctx_t *err = utils__error_ctx();

            logger__log(cfg, LVL_WARN, "Failed to retrieve HTML contents from file system: %s (%d)", err->msg, err->code);

            res.code = 500;
            strcpy(res.msg, "Internal Server Error");

            goto skip_body;
        }

        // If body is NULL, indicates the path was not found. Return 404 page.
        if (!res_body) {
            res.code = 404;
            strcpy(res.msg, "Not Found");

            // Try retrieving not found page.
            char path[MAX_FILE_LEN];
            snprintf(path, sizeof(path), "%s/404.html", cfg->public_dir);
            utils__read_file(path, &res_body);
        } else {
            res.code = 200;
            strcpy(res.msg, "OK");
        }

        res.body = res_body;

skip_body:

        // Set some headers.
        utils__http_header_add(res.headers, &res.headers_cnt, "Server", cfg->server_name);
        utils__http_header_add(res.headers, &res.headers_cnt, "Content-Type", "text/html");
        utils__http_header_add(res.headers, &res.headers_cnt, "Cache-Control", "no-store");

        char *res_full = NULL;

        if (utils__http_response_write(&res, &res_full)) {
            error_ctx_t* err = utils__error_ctx();

            logger__log(cfg, LVL_ERROR, "Failed to write raw HTTP response: %s (%d)", err->msg, err->code);

            close(cl_fd);

            continue;
        }

        // Cleanup both request and response headers now since they're not needed.
        utils__http_header_cleanup(req.headers, req.headers_cnt);
        utils__http_header_cleanup(res.headers, res.headers_cnt);

        // Free body buffer.
        if (res_body) {
            free(res_body);
            res_body = NULL;
        }

        if (!res_full) {
            logger__log(cfg, LVL_ERROR, "Failed to generate response.", ret);

            close(cl_fd);

            continue;
        }

        logger__log(cfg, LVL_TRACE, "Sending back code: %d", res.code);

        int s = send(cl_fd, res_full, strlen(res_full), 0);

        if (s < 0) {
            logger__log(cfg, LVL_ERROR, "Failed to send response.");

            close(cl_fd);

            continue;
        }

        close(cl_fd);
    }

    if (cfg->thread_type == THREAD_TYPE_PER_SOCK)
        close(sock_fd);

    free(tctx);

    pthread_exit(NULL);
}