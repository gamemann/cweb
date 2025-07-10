#include "common.h"

int http__is_header(const char* line) {
    return strstr(line, ":") != NULL;
}

const char* http__header_get(http_header_t* headers, int header_cnt, const char* name) {
    for (int i = 0; i < header_cnt; i++) {
        http_header_t *header = &headers[i];

        if (!header)
            break;

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

    header->value = strdup(value);

    (*header_cnt)++;

    return 0;
}

int http__header_parse_raw(http_header_t* headers, int* header_cnt, const char* line) {
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

    return http__header_add(headers, header_cnt, name, value);
}

int http__header_cleanup(http_header_t* headers, int header_cnt) {
    for (int i = 0; i < header_cnt; i++) {
        http_header_t *t = &headers[i];

        if (!t)
            continue;

        if (t->value) {
            free(t->value);
            t->value = NULL;
        }
    }

    return 0;
}