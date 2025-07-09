#pragma once

#include <config.h>

#include <utils/constants.h>
#include <utils/int_types.h>
#include <utils/helpers.h>

#include <cweb/ctx/ctx.h>

#include <cweb/logger/logger.h>

#include <string.h>

struct http_header {
    char name[MAX_NAME_LEN];
    char value[4096];
} typedef http_header_t;

struct http_request {
    char method[MAX_NAME_LEN];
    char path[MAX_FILE_LEN];
    char version[MAX_NAME_LEN];

    http_header_t headers[MAX_HEADERS];
    int headers_cnt;

    char* body;
} typedef http_request_t;

struct http_response {
    int code;

    http_header_t headers[MAX_HEADERS];
    int headers_cnt;

    char* body;
} typedef http_response_t;