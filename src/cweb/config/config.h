#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <utils/constants.h>
#include <utils/int_types.h>
#include <utils/helpers.h>

#include <json-c/json.h>

enum thread_type {
    THREAD_TYPE_GLOBAL_SOCK = 0,
    THREAD_TYPE_PER_SOCK
} typedef thread_type_t;

struct config {
    int log_lvl;
    char log_file[MAX_FILE_LEN];

    char bind_addr[MAX_IP_LEN];
    int bind_port;

    char server_name[MAX_NAME_LEN];
    char public_dir[MAX_FILE_LEN];

    int threads;
    int thread_type;

    char allowed_hosts[MAX_ALLOWED_HOSTS][MAX_IP_LEN];
    int allowed_hosts_cnt;

    char allowed_user_agents[MAX_ALLOWED_USER_AGENTS][255];
    int allowed_user_agents_cnt;
} typedef config_t;

struct config_overrides {
    int log_lvl;
    char log_file[MAX_FILE_LEN];

    char bind_addr[MAX_IP_LEN];
    int bind_port;
} typedef config_overrides_t;

void cfg__defaults(config_t* cfg);
int cfg__load(config_t* cfg, const char* cfg_path, int load_defaults);
int cfg__parse(config_t* cfg, const char* data);
void cfg__print(config_t* cfg);