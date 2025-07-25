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
    strncpy(cfg->bind_addr, "127.0.0.1", sizeof(cfg->bind_addr));
    cfg->bind_port = 80;

    cfg->log_lvl = LVL_NOTICE;

    utils__str_copy(cfg->log_file, "/var/log/cweb.log", sizeof(cfg->log_file));

    utils__str_copy(cfg->server_name, "CWeb", sizeof(cfg->server_name));
    utils__str_copy(cfg->public_dir, "./public", sizeof(cfg->public_dir));
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
        utils__str_copy(cfg->log_file, json_object_get_string(log_file), sizeof(cfg->log_file));

    // Retrieve bind address.
    json_object *bind_addr = json_object_object_get(root, "bind_addr");

    if (bind_addr)
        utils__str_copy(cfg->bind_addr, json_object_get_string(bind_addr), sizeof(cfg->bind_addr));

    // Retrieve bind port.
    json_object *bind_port = json_object_object_get(root, "bind_port");

    if (bind_port)
        cfg->bind_port = (u16)json_object_get_int(bind_port);

    // Retrieve server name.
    json_object *server_name = json_object_object_get(root, "server_name");

    if (server_name)
        utils__str_copy(cfg->server_name, json_object_get_string(server_name), sizeof(cfg->server_name));

    // Retrieve public HTML directory.
    json_object *public_dir = json_object_object_get(root, "public_dir");

    if (public_dir)
        utils__str_copy(cfg->public_dir, json_object_get_string(public_dir), sizeof(cfg->public_dir));

    // Retrieve threads count.
    json_object *threads = json_object_object_get(root, "threads");

    if (threads)
        cfg->threads = json_object_get_int(threads);

    // Retrieve thread type.
    json_object *thread_type = json_object_object_get(root, "thread_type");

    if (thread_type)
        cfg->thread_type = (thread_type_t)json_object_get_int(thread_type);

    // Retrieve allowed hosts.
    cfg->allowed_hosts_cnt = 0;

    json_object *allowed_hosts = json_object_object_get(root, "allowed_hosts");

    if (allowed_hosts) {
        // Handle array.
        if (json_object_get_type(allowed_hosts) == json_type_array) {
            int len = json_object_array_length(allowed_hosts);

            json_object *host;

            for (int i = 0; i < len; i++) {
                if (i >= MAX_ALLOWED_HOSTS)
                    break;
                
                host = json_object_array_get_idx(allowed_hosts, i);

                utils__str_copy(cfg->allowed_hosts[i], json_object_get_string(host), sizeof(cfg->allowed_hosts[i]));

                cfg->allowed_hosts_cnt++;
            }
        } else {
            // Treat as a single string.
            utils__str_copy(cfg->allowed_hosts[0], json_object_get_string(allowed_hosts), sizeof(cfg->allowed_hosts[0]));

            cfg->allowed_hosts_cnt = 1;
        }
    }

    // Retrieve allowed user agents.
    cfg->allowed_user_agents_cnt = 0;

    json_object *allowed_user_agents = json_object_object_get(root, "allowed_user_agents");

    if (allowed_user_agents) {
        // Handle array.
        if (json_object_get_type(allowed_user_agents) == json_type_array) {
            int len = json_object_array_length(allowed_user_agents);

            json_object *ua;

            for (int i = 0; i < len; i++) {
                if (i >= MAX_ALLOWED_USER_AGENTS)
                    break;

                ua = json_object_array_get_idx(allowed_user_agents, i);

                utils__str_copy(cfg->allowed_user_agents[i], json_object_get_string(ua), sizeof(cfg->allowed_user_agents[i]));

                cfg->allowed_user_agents_cnt++;
            }
        } else {
            // Treat as a single string.
            utils__str_copy(cfg->allowed_user_agents[0], json_object_get_string(allowed_user_agents), sizeof(cfg->allowed_user_agents[0]));

            cfg->allowed_user_agents_cnt = 1;
        }
    }

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

    printf("Log File: %s\n", strlen(cfg->log_file) > 0 ? cfg->log_file : "N/A");
    printf("Bind Address: %s\n", cfg->bind_addr);
    printf("Bind Port: %d\n", cfg->bind_port);
    printf("Server Name: %s\n", cfg->server_name);
    printf("Public Directory: %s\n", cfg->public_dir);
    printf("Threads: %d (0 = auto)\n", cfg->threads);
    
    char *thread_type_str = "Global Socket";

    if (cfg->thread_type == THREAD_TYPE_PER_SOCK)
        thread_type_str = "Per Socket";

    printf("Thread Type: %s\n", thread_type_str);

    if (cfg->allowed_hosts_cnt > 0) {
        printf("Allowed Hosts:\n");

        char *host;

        for (int i = 0; i < cfg->allowed_hosts_cnt; i++) {
            host = cfg->allowed_hosts[i];

            printf("\t- %s\n", host);
        }
    }

    if (cfg->allowed_user_agents_cnt > 0) {
        printf("Allowed User Agents:\n");

        char *ua;

        for (int i = 0; i < cfg->allowed_user_agents_cnt; i++) {
            ua = cfg->allowed_user_agents[i];

            printf("\t- %s\n", ua);
        }
    }
}