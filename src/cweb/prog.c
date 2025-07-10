#include <stdio.h>
#include <stdlib.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <cweb/cli/cli.h>
#include <cweb/config/config.h>
#include <cweb/logger/logger.h>
#include <cweb/server/server.h>

int main(int argc, char** argv) {
    int ret;

    // Load CLI.
    cli_t cli = {0};
    
    // Set default config path.
    strncpy(cli.cfg_path, "./conf.json", sizeof(cli.cfg_path));

    cli__parse(&cli, argc, argv);

    // Load config.
    config_t cfg = {0};

    if ((ret = cfg__load(&cfg, cli.cfg_path, 1)) != 0) {
        logger__log(&cfg, LVL_FATAL, "Failed to load config '%s' (code => %d).", cli.cfg_path, ret);

        return EXIT_FAILURE;
    }

    logger__log(&cfg, LVL_NOTICE, "Config loaded...");

    // Check for printing config.
    if (cli.list) {
        cfg__print(&cfg);

        return EXIT_SUCCESS;
    }

    // Create context.
    ctx_t ctx = {0};
    ctx.cli = &cli;
    ctx.cfg = &cfg;

    // Spin up server.
    if ((ret = server__setup(&ctx)) != 0) {
        logger__log(&cfg, LVL_FATAL, "Failed to setup web server: %d", ret);

        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}