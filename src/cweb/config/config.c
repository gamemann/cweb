#include "config.h"

#include <cweb/logger/logger.h>

void cfg__defaults(config_t* cfg) {
    strncpy(cfg->bind_addr, "127.0.0.1", sizeof(cfg->bind_addr));
    cfg->bind_port = 80;

    cfg->log_lvl = LVL_NOTICE;
    strncpy(cfg->log_file, "/var/log/cweb.log", sizeof(cfg->log_file));

    strncpy(cfg->server_name, "CWeb", sizeof(cfg->server_name));
    strncpy(cfg->public_dir, "./public", sizeof(cfg->public_dir));
}

int cfg__load(config_t* cfg, const char* cfg_path, int load_defaults) {
    int ret;

    // Check for defaults.
    if (load_defaults)
        cfg__defaults(cfg);

    char *buffer = NULL;

    if ((ret = utils__read_file(cfg_path, &buffer)) != 0)
        return ret;

    if ((ret = cfg__parse(cfg, buffer)) != 0)
        return ret;

    free(buffer);

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
        strncpy(cfg->bind_addr, json_object_get_string(bind_addr), sizeof(cfg->bind_addr));

    // Retrieve bind port.
    json_object *bind_port = json_object_object_get(root, "bind_port");

    if (bind_port)
        cfg->bind_port = (u16)json_object_get_int(bind_port);

    // Retrieve server name.
    json_object *server_name = json_object_object_get(root, "server_name");

    if (server_name)
        strncpy(cfg->server_name, json_object_get_string(server_name), sizeof(cfg->server_name));

    // Retrieve public HTML directory.
    json_object *public_dir = json_object_object_get(root, "public_dir");

    if (public_dir)
        strncpy(cfg->public_dir, json_object_get_string(public_dir), sizeof(cfg->public_dir));

    return 0;
}

void cfg__print(config_t* cfg) {
    printf("Config settings:\n");

    const char *log_lvl_str = logger__get_lvl_str((log_level_t)cfg->log_lvl);
    printf("Log Level: %s (%d)\n", log_lvl_str, cfg->log_lvl);

    printf("Log File: %s\n", strlen(cfg->log_file) > 0 ? cfg->log_file : "N/A");
    printf("Bind Address: %s\n", cfg->bind_addr);
    printf("Bind Port: %d\n", cfg->bind_port);
    printf("Server Name: %s\n", cfg->server_name);
    printf("Public Directory: %s\n", cfg->public_dir);
}