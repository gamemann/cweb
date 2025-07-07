#include "http.h"

const char* http__header_get(http_header_t* headers, int header_cnt, const char* name) {
    for (int i = 0; i < header_cnt; i++) {
        http_header_t *header = &headers[i];

        if (strncmp(header->name, name, sizeof(header->name)) != 0)
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

    strncpy(header->value, name, sizeof(header->value) - 1);
    header->value[sizeof(header->value) - 1] = '\0';

    (*header_cnt)++;

    return 0;
}

int http__request_parse_info(http_request_t* req, const char* line) {
    return 0;
}

int http__request_is_header(const char* line) {
    return strstr(line, ":") != NULL;
}

int http__request_parse_header(http_request_t* req, const char* line) {
    // We need to make another copy.
    char *line_cpy = malloc(strlen(line) + 1);
    strcpy(line_cpy, line);

    char *ptr = strtok(line_cpy, ":");

    // Make sure a colon exists.
    if (ptr == NULL)
        return 1;

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
    if (line_cpy) {
        free(line_cpy);
        line_cpy = NULL;
    }

    return http__header_add(req->headers, &req->headers_cnt, name, value);
}

int http__request_parse(ctx_t* ctx, http_request_t* req, const char* buffer) {
    int ret;

    char *buffer_cpy = malloc(strlen(buffer) + 1);

    if (!buffer_cpy)
        return 1;

    strcpy(buffer_cpy, buffer);

    char *line = strtok(buffer_cpy, "\r\n");
    int line_num = 1;

    while (line != NULL) {
        // Check for request information (first line).
        if (line_num == 1) {
            if ((ret = http__request_parse_info(req, (const char*)line)) != 0)
                logger__log(ctx->cfg, LVL_ERROR, "Failed to parse HTTP request information (code => %d)", ret);

            goto line_end;
        }

        // Check for header.
        if (http__request_is_header(line)) {
            if ((ret = http__request_parse_header(req, line)) != 0)
                logger__log(ctx->cfg, LVL_ERROR, "Failed to parse HTTP request header (code => %d)", ret);
        }

line_end:
        line = strtok(NULL, "\r\n");
        line_num++;
    }

    // Free buffer copy.
    if (buffer_cpy) {
        free(buffer_cpy);
        buffer_cpy = NULL;
    }

    return 0;
}