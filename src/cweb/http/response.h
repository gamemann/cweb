#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/split.h>
#include <utils/string/trim.h>


#include <cweb/http/common.h>

#include <cweb/ctx/ctx.h>

#include <cweb/logger/logger.h>

#include <string.h>

struct http_response {
    char version[24];
    int code;
    char msg[MAX_NAME_LEN];

    http_header_t headers[MAX_HEADERS];
    int headers_cnt;

    char* body;
} typedef http_response_t;

int http__response_header_parse(http_response_t* res, char* line);
char* http__response_write(http_response_t* res);