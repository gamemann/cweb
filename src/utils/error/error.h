#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/copy.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#define ERR_MSG_MAX_LEN 1024

#define ERR_SET(code, msg, ...) utils__error_set((code), __func__, (msg), ##__VA_ARGS__)

struct error_ctx {
    int code;
    char msg[ERR_MSG_MAX_LEN];
    char func[MAX_NAME_LEN];
} typedef error_ctx_t;

void utils__error_set(int code, const char* func, const char* msg, ...);
error_ctx_t* utils__error_ctx();