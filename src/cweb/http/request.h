#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>
#include <utils/helpers.h>

#include <cweb/http/common.h>

#include <cweb/ctx/ctx.h>

#include <cweb/logger/logger.h>

#include <string.h>

struct http_request {
    char method[MAX_NAME_LEN];
    char path[MAX_FILE_LEN];
    char version[24];

    http_header_t headers[MAX_HEADERS];
    int headers_cnt;

    char* body;
} typedef http_request_t;

int http__request_parse_info(http_request_t* req, char* line);
int http__request_header_parse(http_request_t* req, char* line);
int http__request_parse(ctx_t* ctx, http_request_t* req, char* buffer);
char* http__request_write(http_request_t* req);