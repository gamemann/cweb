#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <cweb/config/config.h>
#include <cweb/cli/cli.h>

struct ctx {
    cli_t* cli;
    config_t* cfg;
} typedef ctx_t;