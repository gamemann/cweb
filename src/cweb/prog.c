#include <stdio.h>
#include <stdlib.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <cweb/cli/cli.h>
#include <cweb/config/config.h>
#include <cweb/logger/logger.h>
#include <cweb/server/server.h>

#include <signal.h>

int cont = 1;

void sig_hdl(int tmp) {
    cont = 0;
}

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

    // Retrieve thread count.
    int threads = cfg.threads;

    // If below 1 (usually not specified), retrieve amount of threads from host.
    if (threads < 1)
        threads = get_nprocs();

    // Create context.
    ctx_t ctx = {0};
    ctx.cli = &cli;
    ctx.cfg = &cfg;

    logger__log(&cfg, LVL_INFO, "Setting up web server with %d threads...", threads);

    // Spin up web server.
    if ((ret = server__setup(&ctx, threads)) != 0) {
        logger__log(&cfg, LVL_FATAL, "Failed to setup web server: %d", ret);

        return EXIT_FAILURE;
    }

    // Create signal.
    signal(SIGINT, sig_hdl);
    signal(SIGTERM, sig_hdl);

    // Loop every second until signal is caught.
    while (cont) {
        sleep(1);
    }

    logger__log(&cfg, LVL_NOTICE, "Found shutdown signal. Shutting down web server...");

    // Attempt to shut down web server.
    if ((ret = server__shutdown(threads)) != 0) {
        logger__log(&cfg, LVL_ERROR, "Failed to shutdown all web server threads (%d).", ret);

        return EXIT_FAILURE;
    }

    logger__log(&cfg, LVL_INFO, "Successfully shut down web server. Bye!");
    
    return EXIT_SUCCESS;
}