#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/error/error.h>

#include <cweb/config/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <time.h>

#include <errno.h>

enum log_level {
    LVL_FATAL = 1,
    LVL_ERROR,
    LVL_WARN,
    LVL_NOTICE,
    LVL_INFO,
    LVL_DEBUG,
    LVL_TRACE
} typedef log_level_t;

const char* logger__get_lvl_str(log_level_t lvl);
int logger__log_raw(log_level_t lvl, log_level_t req_lvl, const char* log_path, const char* msg, va_list args);
void logger__log(config_t* cfg, log_level_t req_lvl, const char* msg, ...);