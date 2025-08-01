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

        // Check allowed hosts.
        if (cfg->allowed_hosts_cnt > 0) {
            const char *host_full = utils__http_header_get(req.headers, req.headers_cnt, "Host");

            if (!host_full) {
                res.code = 403;
                strcpy(res.msg, "Forbidden");

                logger__log(cfg, LVL_NOTICE, "[ALLOWED_HOSTS] Failed to retrieve 'Host' header.");

                goto skip_body;
            }

            // We need to split the host by colon. So let's clone the host full and retrieve that.
            char *host_full_cpy = strdup(host_full);

            if (!host_full_cpy) {
                res.code = 403;
                strcpy(res.msg, "Forbidden");

                logger__log(cfg, LVL_NOTICE, "[ALLOWED_HOSTS] Failed to copy host full.");

                goto skip_body;
            }

            // Split by colon (<host>:<port>).
            // To Do: Should we check if a colon exists? If a colon doesn't exist, host below will return full string regardless.
            char *saved_ptr;
            char *host = strtok_r(host_full_cpy, ":", &saved_ptr);

            int allowed = 0;
            char *t;

            for (int i = 0; i < cfg->allowed_hosts_cnt; i++) {
                t = cfg->allowed_hosts[i];

                if (strcmp(host, t) == 0) {
                    allowed = 1;

                    break;
                }
            }

            // Free host full copy.
            free(host_full_cpy);

            if (!allowed) {
                res.code = 403;
                strcpy(res.msg, "Forbidden");

                logger__log(cfg, LVL_NOTICE, "[ALLOWED_HOSTS] Host not allowed.");

                goto skip_body;
            }
        }

        // Check allowed user agents.
        if (cfg->allowed_user_agents_cnt > 0) {
            const char *ua = utils__http_header_get(req.headers, req.headers_cnt, "User-Agent");

            if (!ua) {
                res.code = 403;
                strcpy(res.msg, "Forbidden");

                goto skip_body;
            }

            int allowed = 0;
            char *t;

            for (int i = 0; i < cfg->allowed_user_agents_cnt; i++) {
                t = cfg->allowed_user_agents[i];

                if (strcmp(ua, t) == 0) {
                    allowed = 1;

                    break;
                }
            }

            if (!allowed) {
                res.code = 403;
                strcpy(res.msg, "Forbidden");

                goto skip_body;
            }
        }

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