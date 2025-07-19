#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/copy.h>

#include <utils/pointer/set.h>

#include <utils/file/read.h>

#include <json-c/json.h>

enum thread_type {
    THREAD_TYPE_GLOBAL_SOCK = 0,
    THREAD_TYPE_PER_SOCK
} typedef thread_type_t;

struct cfg_route_proxy {
    char* addr;

    char** hdrs;
    int hdrs_cnt;
} typedef cfg_route_proxy_t;

struct cfg_route {
    char* route_regex;
    char* route_plain;

    char* serve_file;
    char* serve_dir;
    char* serve_type;

    char** res_hdrs;
    int res_hdrs_cnt;

    cfg_route_proxy_t proxy;

    char** ua;
    int ua_cnt;
} typedef cfg_route_t;

struct cfg_site {
    char** domains;
    int domains_cnt;

    cfg_route_t** routes;
    int routes_cnt;
} typedef cfg_site_t;

struct config {
    int log_lvl;
    char* log_file;

    char* bind_addr;
    int bind_port;

    char* server_name;
    char* public_dir;

    int threads;
    int thread_type;

    cfg_site_t** sites;
    int sites_cnt;
} typedef config_t;

struct config_overrides {
    int log_lvl;
    char* log_file;

    char* bind_addr;
    int bind_port;
} typedef config_overrides_t;

void cfg__defaults(config_t* cfg);
int cfg__load(config_t* cfg, const char* cfg_path, int load_defaults);
int cfg__parse(config_t* cfg, const char* data);
int cfg__close(config_t** cfg);
void cfg__print(config_t* cfg);