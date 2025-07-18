#include "config.h"

#include <cweb/logger/logger.h>

/**
 * Sets the config's defaults.
 * 
 * @param cfg A pointer to the config.
 * 
 * @return void
 */
void cfg__defaults(config_t* cfg) {
    utils__ptr_set_str(&cfg->bind_addr, "0.0.0.0");
    cfg->bind_port = 80;

    cfg->log_lvl = LVL_NOTICE;

    utils__ptr_set_str(&cfg->log_file, "/var/log/cweb.log");

    utils__ptr_set_str(&cfg->server_name, "CWeb");
    utils__ptr_set_str(&cfg->public_dir, "./public");
}

/**
 * Loads the config.
 * 
 * @param cfg A pointer to the config.
 * @param cfg_path The path to the config file to load.
 * @param load_defaults Whether to load config defaults or not.
 * 
 * @return 0 on success or error from utils__read_file() or cfg__parse().
 */
int cfg__load(config_t* cfg, const char* cfg_path, int load_defaults) {
    // Check for defaults.
    if (load_defaults)
        cfg__defaults(cfg);

    char *buffer = NULL;

    if (utils__read_file(cfg_path, &buffer))
        return 1;

    if (cfg__parse(cfg, buffer))
        return 1;

    free(buffer);

    return 0;
}

/**
 * Parses the config contents using JSON.
 * 
 * @param cfg A pointer to the config.
 * @param data The JSON data to parse.
 * 
 * @return 0 on success or 1 on error.
 */
int cfg__parse(config_t* cfg, const char* data) {
    json_object *root, *obj;

    // Load root JSON contents.
    root = json_tokener_parse(data);

    if (!root) {
        ERR_SET(1, "Failed to parse JSON.");
        
        return 1;
    }

    // Retrieve log level.
    json_object *log_lvl = json_object_object_get(root, "log_lvl");

    if (log_lvl)
        cfg->log_lvl = json_object_get_int(log_lvl);

    // Retrieve log file.
    json_object *log_file = json_object_object_get(root, "log_file");

    if (log_file)
        utils__ptr_set_str(&cfg->log_file, json_object_get_string(log_file));

    // Retrieve bind address.
    json_object *bind_addr = json_object_object_get(root, "bind_addr");

    if (bind_addr)
        utils__ptr_set_str(&cfg->bind_addr, json_object_get_string(bind_addr));

    // Retrieve bind port.
    json_object *bind_port = json_object_object_get(root, "bind_port");

    if (bind_port)
        cfg->bind_port = (u16)json_object_get_int(bind_port);

    // Retrieve server name.
    json_object *server_name = json_object_object_get(root, "server_name");

    if (server_name)
        utils__ptr_set_str(&cfg->server_name, json_object_get_string(server_name));

    // Retrieve public HTML directory.
    json_object *public_dir = json_object_object_get(root, "public_dir");

    if (public_dir)
        utils__ptr_set_str(&cfg->public_dir, json_object_get_string(public_dir));

    // Retrieve threads count.
    json_object *threads = json_object_object_get(root, "threads");

    if (threads)
        cfg->threads = json_object_get_int(threads);

    // Retrieve thread type.
    json_object *thread_type = json_object_object_get(root, "thread_type");

    if (thread_type)
        cfg->thread_type = (thread_type_t)json_object_get_int(thread_type);

    return 0;
}

/**
 * Closes a config (frees all pointers including itself).
 * @param cfg The config.
 * 
 * @return 0 on success or 1 on error.
 */
int cfg__close(config_t** cfg) {
    if (!cfg || !*cfg) {
        ERR_SET(1, "Config already not allocated.");

        return 1;
    }

    utils__ptr_free((void**) &(*cfg)->bind_addr);

    //utils__ptr_free((void**) cfg);

    return 0;
}

/**
 * Prints the formatted config contents.
 * 
 * @param cfg A pointer to the config.
 * 
 * @return void
 */
void cfg__print(config_t* cfg) {
    printf("Config settings:\n");

    const char *log_lvl_str = logger__get_lvl_str((log_level_t)cfg->log_lvl);
    printf("Log Level: %s (%d)\n", log_lvl_str, cfg->log_lvl);

    printf("Log File: %s\n", cfg->log_file ? cfg->log_file : "N/A");
    printf("Bind Address: %s\n", cfg->bind_addr ? cfg->bind_addr : "N/A");
    printf("Bind Port: %d\n", cfg->bind_port);
    printf("Server Name: %s\n", cfg->server_name ? cfg->server_name : "N/A");
    printf("Public Directory: %s\n", cfg->public_dir ? cfg->public_dir : "N/A");
    printf("Threads: %d (0 = auto)\n", cfg->threads);
    
    char *thread_type_str = "Global Socket";

    if (cfg->thread_type == THREAD_TYPE_PER_SOCK)
        thread_type_str = "Per Socket";

    printf("Thread Type: %s\n", thread_type_str);
}