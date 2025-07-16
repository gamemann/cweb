#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/copy.h>

#include <utils/http/common.h>

struct cli {
    int details;

    char host[MAX_IP_LEN];
    u16 port;

    char domain[HTTP_DOMAIN_MAX_LEN];

    char method[HTTP_METHOD_MAX_LEN];
    char path[HTTP_PATH_MAX_LEN];
    char http_version[HTTP_VERSION_MAX_LEN];

    char ua[HTTP_UA_MAX_LEN];

    char body[4096];

    int threads;

    u64 send_delay;

    int list;
    int help;
} typedef cli_t;

void cli__parse(cli_t* cli, int argc, char **argv);