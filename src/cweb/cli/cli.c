#include "cli.h"

const struct option opts[] = {
    { "cfg", required_argument, NULL, 'c' },

    { "time", required_argument, NULL, 't' },

    { "list", no_argument, NULL, 'l' },
    { "help", no_argument, NULL, 'h' },

    { "log-lvl", required_argument, NULL, 'r' },
    { "log-file", required_argument, NULL, 'f' },

    { "bind-addr", required_argument, NULL, 'b' },
    { "bind-port", required_argument, NULL, 'p' },
    
    { NULL, 0, NULL, 0 }
};

/**
 * Parses CLI arguments.
 * 
 * @param cli A pointer to the CLI structure.
 * @param argc The arguments count.
 * @param argv A pointer to pointer of the arguments.
 * 
 * @return void
 */
void cli__parse(cli_t* cli, int argc, char** argv) {
    int c;

    while ((c = getopt_long(argc, argv, "c:t:lhr:f:b:p:", opts, NULL)) != -1) {
        switch (c) {
            case 'c':
                strncpy(cli->cfg_path, optarg, sizeof(cli->cfg_path));

                break;

            case 't':
                cli->time = atoi(optarg);

                break;

            case 'l':
                cli->list = 1;

                break;

            case 'h':
                cli->help = 1;

                break;

            case 'r':
                cli->o_log_lvl = atoi(optarg);

                break;

            case 'f':
                strncpy(cli->o_log_file, optarg, sizeof(cli->o_log_file));

                break;

            case 'b':
                strncpy(cli->o_bind_addr, optarg, sizeof(cli->o_bind_addr));

                break;

            case 'p':
                cli->o_bind_port = atoi(optarg);

                break;
        }
    }
}