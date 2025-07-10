#include "raw.h"

/**
 * Sets up a raw server.
 * 
 * @param ctx A pointer to the context.
 * 
 * @return 0 on success. 1 on socket setup failure. 2 on bind address conversion failure. 3 on bind error. 4 on listen error.
 */
int server__setup_raw(ctx_t* ctx) {
    int ret;

    // Create new socket.
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        logger__log(ctx->cfg, LVL_ERROR, "Failed to create socket.");

        return 1;
    }

    // Allow reuse of address.
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct in_addr in;

    if (inet_pton(AF_INET, ctx->cfg->bind_addr, &in) != 1) {
        logger__log(ctx->cfg, LVL_ERROR, "Failed to convert bind address to decimal form.");

        close (fd);

        return 2;
    }

    // Generate socket information.
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(ctx->cfg->bind_port);
    sin.sin_addr = in;

    // Bind socket.
    if (bind(fd, (struct sockaddr*)&sin, sizeof(sin)) != 0) {
        logger__log(ctx->cfg, LVL_ERROR, "Failed to bind TCP socket.");

        close(fd);

        return 3;
    }

    // Listen on socket.
    if (listen(fd, SOMAXCONN) != 0) {
        logger__log(ctx->cfg, LVL_ERROR, "Failed to listen on TCP socket.");

        close(fd);

        return 4;
    }

    // Wait for new connections.
    while (1) {
        logger__log(ctx->cfg, LVL_TRACE, "Waiting for new connections...");

        // Create buffer for client.
        struct sockaddr_in cl;
        socklen_t cl_len = sizeof(cl);

        // Accept connection.
        int new_fd = accept(fd, (struct sockaddr*)&cl, &cl_len);

        if (new_fd < 0) {
            logger__log(ctx->cfg, LVL_ERROR, "Failed to accept connection.");

            continue;
        }

        logger__log(ctx->cfg, LVL_TRACE, "Accepted new connection!");

        char buffer[4096];
        
        // Read data.
        ssize_t read = recv(new_fd, buffer, sizeof(buffer) - 1, 0);

        if (read < 0) {
            logger__log(ctx->cfg, LVL_ERROR, "Failed to receive data.");

            close(new_fd);

            continue;
        }

        buffer[read] = '\0';

        logger__log(ctx->cfg, LVL_TRACE, "Read %d bytes from request.", read);

        http_request_t req = {0};

        if ((ret = http__request_parse(ctx, &req, buffer)) != 0) {
            logger__log(ctx->cfg, LVL_ERROR, "Failed to parse request: %d", ret);

            close(new_fd);

            continue;
        }

        logger__log(ctx->cfg, LVL_NOTICE, "Method => %s. Path => %s. Version => %s", req.method, req.path, req.version);

        // Create response.
        http_response_t res = {0};
        strcpy(res.version, "HTTP/1.1");

        // Retrieve HTML contents for body.
        char *res_body = fs__web_get_html(req.path, ctx->cfg->public_dir);

        // If body doesn't exist, set to 404 not found.
        if (!res_body) {
            res.code = 404;
            strcpy(res.msg, "Not Found");

            // Try retrieving not found page.
            char path[MAX_FILE_LEN];
            snprintf(path, sizeof(path), "%s/404.html", ctx->cfg->public_dir);
            utils__read_file(path, &res_body);
        } else {
            res.code = 200;
            strcpy(res.msg, "OK");
        }

        res.body = res_body;

        // Set some headers.
        http__header_add(res.headers, &res.headers_cnt, "Server", ctx->cfg->server_name);
        http__header_add(res.headers, &res.headers_cnt, "Content-Type", "text/html");
        http__header_add(res.headers, &res.headers_cnt, "Cache-Control", "no-store");

        char *res_full = http__response_write(&res);

        // Cleanup both request and response headers now since they're not needed.
        http__header_cleanup(req.headers, req.headers_cnt);
        http__header_cleanup(res.headers, res.headers_cnt);

        // Free body buffer.
        if (res_body) {
            free(res_body);
            res_body = NULL;
        }

        if (!res_full) {
            logger__log(ctx->cfg, LVL_ERROR, "Failed to generate response.", ret);

            close(new_fd);

            continue;
        }

        logger__log(ctx->cfg, LVL_TRACE, "Sending back code: %d", res.code);

        int s = send(new_fd, res_full, strlen(res_full), 0);

        if (s < 0) {
            logger__log(ctx->cfg, LVL_ERROR, "Failed to send response.");

            close(new_fd);

            continue;
        }

        close(new_fd);
    }

    close(fd);

    return 0;
}