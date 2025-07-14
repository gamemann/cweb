#include "thread.h"

/**
 * Spawns a thread that handles raw HTTP requests and packets from socket.
 * 
 * @param ctx The thread context.
 * 
 * @return void
 */
void* server__thread(void* ctx) {
    thread_ctx_t* tctx = (thread_ctx_t*)ctx;

    config_t *cfg = tctx->ctx->cfg;

    int ret;

    logger__log(cfg, LVL_INFO, "Spinning up web server thread #%d (socket FD => %d)...", tctx->id, tctx->sock_fd);

    while (1) {
        logger__log(cfg, LVL_TRACE, "Waiting for new connections...");

        // Create buffer for client.
        struct sockaddr_in cl;
        socklen_t cl_len = sizeof(cl);

        // Accept connection.
        int cl_fd = accept(tctx->sock_fd, (struct sockaddr*)&cl, &cl_len);

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

        if ((ret = http__request_parse(ctx, &req, buffer)) != 0) {
            logger__log(cfg, LVL_ERROR, "Failed to parse request: %d", ret);

            close(cl_fd);

            continue;
        }

        logger__log(cfg, LVL_NOTICE, "Method => %s. Path => %s. Version => %s", req.method, req.path, req.version);

        // Create response.
        http_response_t res = {0};
        strcpy(res.version, "HTTP/1.1");

        // Check allowed hosts.
        if (cfg->allowed_hosts_cnt > 0) {
            const char *host_full = http__header_get(req.headers, req.headers_cnt, "Host");

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
            const char *ua = http__header_get(req.headers, req.headers_cnt, "User-Agent");

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

        // Retrieve HTML contents for body.
        char *res_body = fs__web_get_html(req.path, cfg->public_dir);

        // If body doesn't exist, set to 404 not found.
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
        http__header_add(res.headers, &res.headers_cnt, "Server", cfg->server_name);
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

    free(tctx);

    pthread_exit(NULL);
}