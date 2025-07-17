#include "request.h"

/**
 * Parses the HTTP request information (first line with method, path, and version).
 * 
 * @param req A pointer to the HTTP request.
 * @param line The line to parse.
 * 
 * @return 0 on success or 1 on error.
 */
int utils__http_request_parse_info(http_request_t* req, char* line) {
    char *line_cpy = strdup(line);

    if (!line_cpy) {
        ERR_SET(1, "Failed to copy line.");

        return 1;
    }

    // Make sure we have enough spaces.
    int space_cnt = utils__get_delim_cnt(line_cpy, ' ');

    if (space_cnt < 2) {
        ERR_SET(2, "Space count under 2.");

        return 1;
    }

    // Split string by spaces.
    char **tokens = utils__str_split(line_cpy, ' ');

    if (!tokens) {
        free(line_cpy);

        ERR_SET(3, "Failed to split string.");

        return 1;
    }

    // Check method.
    if (*(tokens) == NULL) {
        free(line_cpy);

        free(tokens);

        ERR_SET(4, "No method or malformed request.");

        return 1;
    }

    // Check path.
    if (*(tokens + 1) == NULL) {
        free(line_cpy);

        free(*(tokens));

        free(tokens);

        ERR_SET(5, "No path or malformed request.");

        return 1;
    }

    // Check HTTP version.
    if (*(tokens + 2) == NULL) {
        free(line_cpy);

        free(*(tokens));
        free(*(tokens + 1));

        free(tokens);

        ERR_SET(6, "No HTTP version or malformed request.");

        return 1;
    }

    // Assign method, path, and version from tokens.
    utils__str_copy(req->method, *(tokens), sizeof(req->method));
    utils__str_copy(req->path, *(tokens + 1), sizeof(req->path));
    utils__str_copy(req->version, *(tokens + 2), sizeof(req->version));

    // Free all tokens.
    for (int i = 0; tokens[i]; i++) {
        free(tokens[i]);
    }

    free(tokens);
    free(line_cpy);

    return 0;
}

/**
 * Parses a HTTP request header.
 * 
 * @param req A pointer to the request.
 * @param line The line to parse.
 * 
 * @return 0 on success or 1 on error.
 */
int utils__http_request_header_parse(http_request_t* req, char* line) {
    return utils__http_header_parse_raw(req->headers, &req->headers_cnt, line);
}

/**
 * Parses a HTTP request.
 * 
 * @param req A pointer to the HTTP request.
 * @param buffer The full HTTP request.
 * 
 * @return 0 on success or 1 on error.
 */
int utils__http_request_parse(http_request_t* req, char* buffer) {
    int ret = 0;

    // Copy buffer (needed for strtok()).
    char *buffer_cpy = strdup(buffer);

    if (!buffer_cpy) {
        ret = 1;

        ERR_SET(1, "Failed to copy buffer.");

        goto exit;
    }

    // Make sure we have two empty lines indicating headers and body.
    char *headers_end = strstr(buffer_cpy, "\r\n\r\n");

    if (!headers_end) {
        ret = 1;

        ERR_SET(2, "HTTP request malformed (missing '\\r\\n\\r\\n)' between headers and body if any).");

        goto exit;
    }

    // Split headers and body by terminating end of headers.
    *headers_end = '\0';

    // Retrieve separated body string.
    char *body = headers_end + 4;

    // Split headers by new line.
    char *save_ptr;
    char *line = strtok_r(buffer_cpy, "\r\n", &save_ptr);
    int line_num = 1;
    
    while (line != NULL) {        
        // The first line includes information on request (method, path, and HTTP version).
        if (line_num == 1) {
            if ((ret = utils__http_request_parse_info(req, line)) != 0) {
                ret = 1;

                goto exit;
            }
        } else {
            // Check for header.
            if (utils__http_is_header(line)) {
                if ((ret = utils__http_request_header_parse(req, line)) != 0) {
                    ret = 1;

                    goto exit;
                }
            }
        }

        line = strtok_r(NULL, "\r\n", &save_ptr);
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
 * @param buffer The buffer to store the raw request text in.
 * 
 * @return 0 on success or 1 on error.
 */
int utils__http_request_write(http_request_t* req, char** buffer) {
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
    *buffer = malloc(len);

    if (!*buffer) {
        ERR_SET(1, "Failed to allocate buffer.");

        return 1;
    }

    // Create offset.
    size_t off = 0;

    // Write response information (method, path, and version).
    off += snprintf(*buffer + off, len - off, "%s %s %s\r\n", req->method, req->path, req->version);

    // Add headers.
    for (int i = 0; i < req->headers_cnt; i++) {
        http_header_t *t = &req->headers[i];

        if (!t->value)
            continue;

        off += snprintf(*buffer + off, len - off, "%s: %s\r\n", t->name, t->value);
    }

    off += snprintf(*buffer + off, len - off, "\r\n");

    if (req->body) {
        size_t body_len = strlen(req->body);

        memcpy(*buffer + off, req->body, body_len);
        off += body_len;
    }

    (*buffer)[off] = '\0';

    return 0;
}