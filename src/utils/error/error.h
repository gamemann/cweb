#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/copy.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERR_MSG_MAX_LEN 1024

struct error_ctx {
    int code;
    char msg[ERR_MSG_MAX_LEN];
    char func[MAX_NAME_LEN];
} typedef error_ctx_t;