#include "server.h"

pthread_t pthreads[MAX_THREADS];
int global_sock_fd = -1;

/**
 * Sets up the HTTP server using TCP cooked sockets.
 * 
 * @param ctx A pointer to the context.
 * @param threads The amount of threads to setup.
 * 
 * @return 0 on success or value of server mode setup.
 */
int server__setup(ctx_t* ctx, int threads) {
    int ret;

    // Create new socket.
    int global_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (global_sock_fd < 0) {
        logger__log(ctx->cfg, LVL_ERROR, "Failed to create socket.");

        return 1;
    }

    // Allow reuse of address.
    int opt = 1;

    setsockopt(global_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Allow reuse port.
    setsockopt(global_sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    struct in_addr in;

    if (inet_pton(AF_INET, ctx->cfg->bind_addr, &in) != 1) {
        logger__log(ctx->cfg, LVL_ERROR, "Failed to convert bind address to decimal form.");

        close (global_sock_fd);

        return 2;
    }

    // Generate socket information.
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(ctx->cfg->bind_port);
    sin.sin_addr = in;

    // Bind socket.
    if (bind(global_sock_fd, (struct sockaddr*)&sin, sizeof(sin)) != 0) {
        logger__log(ctx->cfg, LVL_ERROR, "Failed to bind TCP socket.");

        close(global_sock_fd);

        return 3;
    }

    // Listen on socket.
    if (listen(global_sock_fd, SOMAXCONN) != 0) {
        logger__log(ctx->cfg, LVL_ERROR, "Failed to listen on TCP socket.");

        close(global_sock_fd);

        return 4;
    }

    for (int i = 0; i < threads; i++) {
        // Create thread context.
        thread_ctx_t *tctx = malloc(sizeof(thread_ctx_t));

        if (!tctx)
            return 5;

        tctx->id = i + 1;
        tctx->ctx = ctx;
        tctx->global_sock_fd = global_sock_fd;

        pthread_create(&pthreads[i], NULL, server__thread, (void*)tctx);

        //pthread_join(&pthreads[i], NULL);
    }

    return 0;
}

/**
 * Shuts down web server and threads.
 * 
 * @param threads The amount of threads that were spawned.
 * 
 * @return 0 on success or return of one or more of pthread_cancel() errors.
 */
int server__shutdown(int threads) {
    int ret = 0, tmp;

    for (int i = 0; i < threads; i++) {
        if ((tmp = pthread_cancel(pthreads[i])) != 0 && ret == 0)
            ret = tmp;
    }

    // Attempt to close socket FD.
    close(global_sock_fd);

    return ret;
}