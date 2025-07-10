#include "request.h"

/**
 * Parses the HTTP request information (first line with method, path, and version).
 * 
 * @param req A pointer to the HTTP request.
 * @param line The line to parse.
 * 
 * @return 0 on success, 1 on error due to incorrect space count, 2 on error with splitting by spaces, 3 on error parsing.
 */
int http__request_parse_info(http_request_t* req, char* line) {
    // Make sure we have enough spaces.
    int space_cnt = utils__get_delim_cnt(line, ' ');

    if (space_cnt < 2)
        return 1;

    // Split string by spaces.
    char **tokens = utils__str_split(line, ' ');

    if (!tokens)
        return 2;

    // Make sure we have at least 3 strings to use.
    if (*(tokens) == NULL || *(tokens + 1) == NULL || *(tokens + 2) == NULL)
        return 3;

    // Assign method.
    strncpy(req->method, *(tokens), sizeof(req->method) - 1);
    req->method[sizeof(req->method) - 1] = '\0';

    // Assign path.
    strncpy(req->path, *(tokens + 1), sizeof(req->path) - 1);
    req->path[sizeof(req->path) - 1] = '\0';

    // Assign version.
    strncpy(req->version, *(tokens + 2), sizeof(req->version) - 1);
    req->version[sizeof(req->version) - 1] = '\0';

    // Free tokens.
    for (int i = 0; tokens[i]; i++) {
        free(tokens[i]);
    }

    free(tokens);

    return 0;
}

/**
 * Parses a HTTP request header.
 * 
 * @param req A pointer to the request.
 * @param line The line to parse.
 * 
 * @return 0 on success or code from http__header_parse_raw().
 */
int http__request_header_parse(http_request_t* req, const char* line) {
    return http__header_parse_raw(req->headers, &req->headers_cnt, line);
}

/**
 * Parses a HTTP request.
 * 
 * @param ctx A pointer to the context.
 * @param req A pointer to the HTTP request.
 * @param buffer The full HTTP request.
 * 
 * @return 0 on success. 2 or 3 on malformed HTTP request. 4 on malformed HTTP header.
 */
int http__request_parse(ctx_t* ctx, http_request_t* req, const char* buffer) {
    int ret = 0;

    // Copy buffer (needed for strtok()).
    char *buffer_cpy = strdup(buffer);

    if (!buffer_cpy) {
        ret = 1;

        goto exit;
    }

    // Make sure we have two empty lines indicating headers and body.
    char *headers_end = strstr(buffer_cpy, "\r\n\r\n");

    if (!headers_end) {
        ret = 2;

        goto exit;
    }

    // Split headers and body by terminating end of headers.
    *headers_end = '\0';

    // Retrieve separated body string.
    char *body = headers_end + 4;

    // Split headers by new line.
    char *line = strtok(buffer_cpy, "\r\n");
    int line_num = 1;

    while (line != NULL) {
        // The first line includes information on request (method, path, and HTTP version).
        if (line_num == 1) {
            if ((ret = http__request_parse_info(req, line)) != 0) {
                logger__log(ctx->cfg, LVL_ERROR, "Failed to parse HTTP request information (code => %d)", ret);

                ret = 3;

                break;
            }
        } else {
            // Check for header.
            if (http__is_header(line)) {
                if ((ret = http__request_header_parse(req, line)) != 0) {
                    logger__log(ctx->cfg, LVL_ERROR, "Failed to parse HTTP request header (code => %d)", ret);

                    ret = 4;

                    break;
                }
            }
        }

        line = strtok(NULL, "\r\n");
        line_num++;
    }

    // Copy body.
    req->body = strdup(body);

exit:
    // Free buffer copy if allocated.
    if (buffer_cpy) {
        free(buffer_cpy);
        buffer_cpy = NULL;
    }

    return ret;
}

/**
 * Generates the payload of a plain HTTP request.
 * 
 * @param req A pointer to the HTTP request.
 * 
 * @return A character pointer to the plain HTTP request or NULL on error.
 */
char* http__request_write(http_request_t* req) {
    // We need to determine the full length request.
    size_t len = 0;

    // Start with information (method, path, and version).
    len += strlen(req->method) + 1 + strlen(req->path) + 1 + strlen(req->version) + 2;

    // Loop through headers and add up sizes of name and value.
    for (int i = 0; i < req->headers_cnt; i++) {
        http_header_t *t = &req->headers[i];

        len += strlen(t->name) + 2 + strlen(t->value) + 2;
    }

    // \r\n between headers and body.
    len += 2;

    if (req->body)
        len += strlen(req->body);

    // Null terminator.
    len++;

    // Allocate buffer to store request in.
    char *buffer = malloc(len);

    if (!buffer)
        return NULL;

    // Create offset.
    size_t off = 0;

    // Write response information (method, path, and version).
    off += snprintf(buffer + off, len - off, "%s %s %s\r\n", req->method, req->path, req->version);

    // Add headers.
    for (int i = 0; i < req->headers_cnt; i++) {
        http_header_t *t = &req->headers[i];

        if (!t->value)
            continue;

        off += snprintf(buffer + off, len - off, "%s: %s\r\n", t->name, t->value);
    }

    off += snprintf(buffer + off, len - off, "\r\n");

    if (req->body) {
        size_t body_len = strlen(req->body);

        memcpy(buffer + off, req->body, body_len);
        off += body_len;
    }

    buffer[off] = '\0';

    return buffer;
}