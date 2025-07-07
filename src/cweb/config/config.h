#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <json-c/json.h>

struct config_web_processor {
    
} typedef config_web_processor_t;

struct config {
    int log_lvl;
    char log_file[MAX_FILE_LEN];

    char bind_addr[MAX_IP_LEN];
    int bind_port;
} typedef config_t;

struct config_overrides {
    int log_lvl;
    char log_file[MAX_FILE_LEN];

    char bind_addr[MAX_IP_LEN];
    int bind_port;
} typedef config_overrides_t;

void cfg__defaults(config_t* cfg);
int cfg__load(config_t* cfg, const char* cfg_path, int load_defaults);
int cfg__open(FILE** fp, const char* file_path);
int cfg__read(FILE* fp, char** buffer);
int cfg__parse(config_t* cfg, const char* data);
int cfg__close(FILE* fp);
void cfg__print(config_t* cfg);