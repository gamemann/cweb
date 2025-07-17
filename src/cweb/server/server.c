#include "server.h"

pthread_t pthreads[MAX_THREADS];
int global_sock_fd = -1;

/**
 * Sets up the HTTP server using TCP cooked sockets.
 * 
 * @param ctx A pointer to the context.
 * @param threads The amount of threads to setup.
 * 
 * @return 0 on success or 1 on error.
 */
int server__setup(ctx_t* ctx, int threads) {
    int ret;

    // Check if we need to create a global socket.
    if (ctx->cfg->thread_type == THREAD_TYPE_GLOBAL_SOCK) {
        if (server__socket_setup(&global_sock_fd, ctx->cfg->bind_addr, ctx->cfg->bind_port, 1))
            return 1;
    }

    // Create threads that will be processing requests.
    for (int i = 0; i < threads; i++) {
        // Create thread context.
        thread_ctx_t *tctx = malloc(sizeof(thread_ctx_t));

        if (!tctx) {
            ERR_SET(2, "Failed to allocate thread context on thread #%d.", i + 1);

            return 1;
        }

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

    // Close global socket if set.
    if (global_sock_fd > -1)
        close(global_sock_fd);

    return ret;
}