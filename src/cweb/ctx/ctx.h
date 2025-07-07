#pragma once

#include <config.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <config/config.h>
#include <cli/cli.h>

struct ctx {
    cli_t* cli;
    config_t* cfg;
} typedef ctx_t;