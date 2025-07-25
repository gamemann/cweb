#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/split.h>
#include <utils/string/trim.h>
#include <utils/string/copy.h>

#include <utils/error/error.h>

#include <string.h>

#define HTTP_HOST_MAX_LEN 255
#define HTTP_DOMAIN_MAX_LEN 255
#define HTTP_METHOD_MAX_LEN 6
#define HTTP_PATH_MAX_LEN 255
#define HTTP_VERSION_MAX_LEN 12
#define HTTP_UA_MAX_LEN 255

struct http_header {
    char name[MAX_NAME_LEN];
    char* value;
} typedef http_header_t;

int utils__http_is_header(char* line);
const char* utils__http_header_get(http_header_t* headers, int header_cnt, const char* name);
int utils__http_header_add(http_header_t* headers, int* header_cnt, const char* name, const char* value);
int utils__http_header_parse_raw(http_header_t* headers, int* header_cnt, char* line);
void utils__http_header_cleanup(http_header_t* headers, int header_cnt);