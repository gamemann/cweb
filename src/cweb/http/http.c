#include "http.h"

const char* http__header_get(http_header_t* headers, int header_cnt, const char* name) {
    for (int i = 0; i < header_cnt; i++) {
        http_header_t *header = &headers[i];

        if (strcmp(header->name, name) != 0)
            continue;

        return header->value;
    }

    return NULL;
}

int http__header_add(http_header_t* headers, int* header_cnt, const char* name, const char* value) {
    if (*header_cnt >= MAX_HEADERS)
        return 1;

    http_header_t *header = &headers[*header_cnt];

    strncpy(header->name, name, sizeof(header->name) - 1);
    header->name[sizeof(header->name) - 1] = '\0';

    strncpy(header->value, value, sizeof(header->value) - 1);
    header->value[sizeof(header->value) - 1] = '\0';

    (*header_cnt)++;

    return 0;
}

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

int http__request_is_header(const char* line) {
    return strstr(line, ":") != NULL;
}

int http__request_parse_header(http_request_t* req, const char* line) {
    // We need to make another copy.
    char *line_cpy = strdup(line);

    if (!line_cpy)
        return 1;

    char *ptr = strtok(line_cpy, ":");

    // Make sure a colon exists.
    if (ptr == NULL)
        return 2;

    // Retrieve header name.
    char name[MAX_NAME_LEN];
    strncpy(name, ptr, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    ptr = strtok(NULL, ":");

    // Retrieve header value.
    char value[4096];
    strncpy(value, ptr, sizeof(value) - 1);
    value[sizeof(value) - 1] = '\0';

    // Free line copy.
    free(line_cpy);
    line_cpy = NULL;

    return http__header_add(req->headers, &req->headers_cnt, name, value);
}

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
            if (http__request_is_header(line)) {
                if ((ret = http__request_parse_header(req, line)) != 0) {
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