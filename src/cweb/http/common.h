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
    char* value;
} typedef http_header_t;

int http__is_header(const char* line);
const char* http__header_get(http_header_t* headers, int header_cnt, const char* name);
int http__header_add(http_header_t* headers, int* header_cnt, const char* name, const char* value);
int http__header_parse_raw(http_header_t* headers, int* header_cnt, const char* line);
int http__header_cleanup(http_header_t* headers, int header_cnt);
