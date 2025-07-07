#include "config.h"

#include <logger/logger.h>

void cfg__defaults(config_t* cfg) {
    strncpy(cfg->bind_addr, "127.0.0.1", sizeof(cfg->bind_addr));
    cfg->bind_port = 80;

    cfg->log_lvl = LVL_NOTICE;
    strncpy(cfg->log_file, "/var/log/cweb.log", sizeof(cfg->log_file));
}

int cfg__load(config_t* cfg, const char* cfg_path, int load_defaults) {
    FILE *fp = NULL;
    int ret;

    // Check for defaults.
    if (load_defaults)
        cfg__defaults(cfg);

    if ((ret = cfg__open(&fp, cfg_path)) != 0)
        return ret;

    char *buffer = NULL;

    if ((ret = cfg__read(fp, &buffer)) != 0)
        return ret;

    if ((ret = cfg__parse(cfg, buffer)) != 0)
        return ret;

    free(buffer);

    if ((ret = cfg__close(fp)) != 0)
        return ret;

    return 0;
}

int cfg__open(FILE** fp, const char* file_path) {
    if (*fp != NULL) {
        fclose(*fp);

        *fp = NULL;
    }

    *fp = fopen(file_path, "r");

    if (*fp == NULL)
        return 1;

    return 0;
}

int cfg__read(FILE* fp, char** buffer) {
    fseek(fp, 0, SEEK_END);
    long file_sz = ftell(fp);
    rewind(fp);

    if (file_sz <= 0)
        return 1;

    *buffer = malloc(file_sz + 1);

    if (*buffer == NULL)
        return 2;

    size_t read = fread(*buffer, 1, file_sz, fp);

    (*buffer)[read] = '\0';

    return 0;
}

int cfg__parse(config_t* cfg, const char* data) {
    json_object *root, *obj;

    // Load root JSON contents.
    root = json_tokener_parse(data);

    if (!root)
        return 1;

    // Retrieve log level.
    json_object *log_lvl = json_object_object_get(root, "log_lvl");

    if (log_lvl)
        cfg->log_lvl = json_object_get_int(log_lvl);

    // Retrieve log file.
    json_object *log_file = json_object_object_get(root, "log_file");

    if (log_file)
        strncpy(cfg->log_file, json_object_get_string(log_file), sizeof(cfg->log_file));

    // Retrieve bind address.
    json_object *bind_addr = json_object_object_get(root, "bind_addr");

    if (bind_addr)
        strncpy(cfg->bind_addr, json_object_get_string(bind_addr), sizeof(cfg->log_file));

    // Retrieve b ind port.
    json_object *bind_port = json_object_object_get(root, "bind_port");

    if (bind_port)
        cfg->bind_port = (u16)json_object_get_int(bind_port);

    return 0;
}

int cfg__close(FILE* fp) {
    if (!fp)
        return 0;

    return fclose(fp);
}

void cfg__print(config_t* cfg) {
    printf("Config settings:\n");

    // Log level.
    const char *log_lvl_str = logger__get_lvl_str((log_level_t)cfg->log_lvl);
    printf("Log Level: %s (%d)\n", log_lvl_str, cfg->log_lvl);

    // Log file.
    printf("Log File: %s\n", strlen(cfg->log_file) > 0 ? cfg->log_file : "N/A");

    // Bind address.
    printf("Bind Address: %s\n", cfg->bind_addr);

    // Bind port.
    printf("Bind Port: %d\n", cfg->bind_port);
}