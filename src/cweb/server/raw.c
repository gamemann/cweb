#include "raw.h"

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

        char buffer[4096];
        
        // Read data.
        ssize_t read = recv(new_fd, buffer, sizeof(buffer) - 1, 0);

        if (read < 0) {
            logger__log(ctx->cfg, LVL_ERROR, "Failed to receive data.");

            close(new_fd);

            continue;
        }

        buffer[read] = '\0';

        http_request_t req = {0};

        if ((ret = http__request_parse(ctx, &req, buffer)) != 0) {
            logger__log(ctx->cfg, LVL_ERROR, "Failed to parse request: %d", ret);

            close(new_fd);

            continue;
        }

        // Create response.
        http_response_t res = {0};
        res.code = 200;
        strcpy(res.msg, "OK");
        strcpy(res.version, "HTTP/1.1");
        res.body = "<html><body>OK!</body></html>";

        char *res_full = http__response_write(&res);

        if (!res_full) {
            logger__log(ctx->cfg, LVL_ERROR, "Failed to generate response.", ret);

            close(new_fd);

            continue;
        }

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