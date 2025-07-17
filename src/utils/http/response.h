#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/split.h>
#include <utils/string/trim.h>

#include <utils/error/error.h>

#include <utils/http/common.h>

#include <string.h>

struct http_response {
    char version[24];
    int code;
    char msg[MAX_NAME_LEN];

    http_header_t headers[MAX_HEADERS];
    int headers_cnt;

    char* body;
} typedef http_response_t;

int utils__http_response_header_parse(http_response_t* res, char* line);
int utils__http_response_write(http_response_t* res, char** buffer);