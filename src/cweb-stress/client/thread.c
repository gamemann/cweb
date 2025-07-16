#include "thread.h"

/**
 * The client thread's main function.
 * 
 * @param tmp The thread context (passed as void).
 * 
 * @return void
 */
void* client__thread_main(void* tmp) {
    thread_ctx_t *tctx = (thread_ctx_t*)tmp;

    int ret;

    // Create HTTP request and body buffer now so we can use goto.
    char *body = NULL, *buffer = NULL;

    // Let's construct the HTTP request now.
    http_request_t req = {0};

    strcpy(req.method, tctx->method);
    strcpy(req.path, tctx->path);
    strcpy(req.version, tctx->http_version);

    // Add Host header.
    utils__http_header_add(req.headers, &req.headers_cnt, "Host", tctx->domain);

    // Add user agent header if set.
    if (strlen(tctx->ua) > 0)
        utils__http_header_add(req.headers, &req.headers_cnt, "User-Agent", tctx->ua);

    // Create body.
    body = strdup(tctx->body);

    req.body = body;

    // Create raw HTTP request.
    buffer = utils__http_request_write(&req);

    if (!buffer) {
        fprintf(stderr, "[T %d] Failed to construct and write HTTP request.\n", tctx->id);

        goto thread_exit;
    }

    // Resolve host.
    struct sockaddr_in sin = {0};
    struct addrinfo hints = {0}, *res = NULL;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%u", tctx->port);

    ret = getaddrinfo(tctx->host, port_str, &hints, &res);

    if (ret != 0 || !res) {
        fprintf(stderr, "[T %d] Failed to resolve host '%s': %s\n", tctx->id, tctx->host, gai_strerror(ret));

        goto thread_exit;
    }

    memcpy(&sin, res->ai_addr, sizeof(struct sockaddr_in));

    freeaddrinfo(res);

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((struct sockaddr_in *)res->ai_addr)->sin_addr, ip_str, sizeof(ip_str));

    printf("[T %d] Resolved '%s' to '%s:%u'\n", tctx->id, tctx->host, ip_str, ntohs(((struct sockaddr_in *)res->ai_addr)->sin_port));

    // Create loop to send HTTP requests.
    while (1) {
        // Create socket to send HTTP request on.
        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

        if (sock_fd < 0) {
            sleep(1);

            continue;
        }

        if (connect(sock_fd, (struct sockaddr *)&sin, sizeof(sin)) != 0) {
            fprintf(stderr, "[T %d] Failed to connect to host using connect(): %s\n", tctx->id, strerror(errno));

            close(sock_fd);

            sleep(1);

            continue;
        }

        int b = send(sock_fd, buffer, strlen(buffer), 0);

        if (b < 1) {
            fprintf(stderr, "[T %d] Failed to send data on socket using send().\n", tctx->id);

            close(sock_fd);

            sleep(1);

            continue;
        }

#ifdef ALLOW_DETAILS
        if (tctx->details)
            printf("[T %d] Sent HTTP request (%d bytes)...\n", tctx->id, b);
#endif

        // Close socket.
        close(sock_fd);
       
#ifdef ALLOW_THREAD_DELAY
        if (tctx->send_delay > 0)
            usleep(tctx->send_delay);
#endif
    }

thread_exit:
    // Free body if needed.
    if (body) {
        free(body);

        body = NULL;
    }

    // Free buffer if needed.
    if (buffer) {
        free(buffer);

        buffer = NULL;
    }
    
    // Free thread's context.
    free(tctx);

    pthread_exit(NULL);
}