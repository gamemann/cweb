#include <stdio.h>
#include <stdlib.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/string/copy.h>

#include <cweb/cli/cli.h>
#include <cweb/config/config.h>
#include <cweb/logger/logger.h>
#include <cweb/server/server.h>

#include <signal.h>

#include <time.h>

int cont = 1;

void sig_hdl(int tmp) {
    cont = 0;
}

int main(int argc, char** argv) {
    int ret;

    // Load CLI.
    cli_t cli = {0};
    
    // Set default config path.
    utils__str_copy(cli.cfg_path, "./conf.json", sizeof(cli.cfg_path));

    cli__parse(&cli, argc, argv);

    // Check for help menu.
    if (cli.help) {
        printf("Usage: cweb [OPTIONS]\n\n");

        printf("  -c, --cfg <val>          Path to the runtime config file (default: ./conf.json).\n");
        printf("  -t, --time <val>         If set, runs the web server for this long in seconds before exiting.\n");
        printf("  -l, --list               Print the contents of the runtime config and exit.\n");
        printf("  -h, --help               Show this help message and exit.\n");
        printf("  -r, --log-lvl <val>      Override the log level value from the config.\n");
        printf("  -f, --log-file <val>     Override the log file path from the config.\n");
        printf("  -b, --bind-addr <val>    Override the bind address from the config.\n");
        printf("  -p, --bind-port <val>    Override the bind port from the config.\n\n");

        return EXIT_SUCCESS;
    }

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

    time_t start_time = time(NULL);
    time_t end_time = 0;

    if (cli.time > 0)
        end_time = start_time + cli.time;

    // Loop every second until signal is caught.
    while (cont) {
        // Check if we need to exit due to time argument.
        if (end_time > 0) {
            time_t cur_time = time(NULL);

            if (cur_time > end_time) {
                logger__log(&cfg, LVL_NOTICE, "Exceeded %d seconds of runtime...", cli.time);

                break;
            }
        }

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