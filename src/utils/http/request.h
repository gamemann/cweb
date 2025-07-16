#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/split.h>
#include <utils/string/trim.h>

#include <utils/http/common.h>

#include <string.h>

struct http_request {
    char method[MAX_NAME_LEN];
    char path[MAX_FILE_LEN];
    char version[24];

    http_header_t headers[MAX_HEADERS];
    int headers_cnt;

    char* body;
} typedef http_request_t;

int utils__http_request_parse_info(http_request_t* req, char* line);
int utils__http_request_header_parse(http_request_t* req, char* line);
int utils__http_request_parse(http_request_t* req, char* buffer);
char* utils__http_request_write(http_request_t* req);