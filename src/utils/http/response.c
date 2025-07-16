#include "response.h"

/**
 * Parses a HTTP response header.
 * 
 * @param res A pointer to the HTTP response.
 * @param line The raw header line.
 * 
 * @return The return of utils__http_header_parse_raw().
 */
int utils__http_response_header_parse(http_response_t* res, char* line) {
    return utils__http_header_parse_raw(res->headers, &res->headers_cnt, line);
}

/**
 * Generates a plain HTTP response.
 * 
 * @param res A pointer to the HTTP response.
 * 
 * @return A character pointer to the plain HTTP response or NULL on error.
 */
char* utils__http_response_write(http_response_t* res) {
    // We need to determine the full length of the response.
    size_t len = 0;

    // Start with first line (version, code, and message).
    len += strlen(res->version) + 1 + 3 + 1 + strlen(res->msg) + 2;

    for (int i = 0; i < res->headers_cnt; i++) {
        http_header_t *t = &res->headers[i];

        if (!t->value)
            continue;

        len += strlen(t->name) + 2 + strlen(t->value) + 2;
    }

    len += 2;

    if (res->body)
        len += strlen(res->body);

    // Null terminator.
    len += 1;

    char *buffer = malloc(len);

    if (!buffer)
        return NULL;

    size_t off = 0;

    off = snprintf(buffer + off, len - off, "%s %d %s\r\n", res->version, res->code, res->msg);

    for (int i = 0; i < res->headers_cnt; i++) {
        http_header_t *t = &res->headers[i];

        if (!t->value)
            continue;
        
        off += snprintf(buffer + off, len - off, "%s: %s\r\n", t->name, t->value);
    }

    off += snprintf(buffer + off, len - off, "\r\n");

    if (res->body) {
        size_t body_len = strlen(res->body);
        memcpy(buffer + off, res->body, body_len);
        off += body_len;
    }

    buffer[off] = '\0';

    return buffer;
}