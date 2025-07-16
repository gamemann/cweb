#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/copy.h>

#include <getopt.h>

struct cli {
    char cfg_path[MAX_FILE_LEN];

    int time;

    int list;
    int help;

    int o_log_lvl;
    char o_log_file[MAX_FILE_LEN];
    char o_bind_addr[MAX_IP_LEN];
    int o_bind_port;
} typedef cli_t;

void cli__parse(cli_t* cli, int argc, char** argv);