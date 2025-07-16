#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/split.h>
#include <utils/string/trim.h>

#include <cweb/logger/logger.h>

#include <string.h>

struct http_header {
    char name[MAX_NAME_LEN];
    char* value;
} typedef http_header_t;

int utils__http_is_header(char* line);
const char* utils__http_header_get(http_header_t* headers, int header_cnt, const char* name);
int utils__http_header_add(http_header_t* headers, int* header_cnt, const char* name, const char* value);
int utils__http_header_parse_raw(http_header_t* headers, int* header_cnt, char* line);
void utils__http_header_cleanup(http_header_t* headers, int header_cnt);