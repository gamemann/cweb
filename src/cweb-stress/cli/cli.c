#include "cli.h"

const struct option opts[] = {
    { "details", no_argument, NULL, 'z' },

    { "host", required_argument, NULL, 'i' },
    { "port", required_argument, NULL, 'p' },

    { "domain", required_argument, NULL, 'd' },

    { "method", required_argument, NULL, 'm' },
    { "path", required_argument, NULL, 'r' },
    { "http-version", required_argument, NULL, 'v' },

    { "ua", required_argument, NULL, 'u' },

    { "body", required_argument, NULL, 'b' },

    { "threads", required_argument, NULL, 't' },
    { "send-delay", required_argument, NULL, 's' },

    { "list", no_argument, NULL, 'l' },
    { "help", no_argument, NULL, 'h' },

    { NULL, 0, NULL, 0 }
};

void cli__parse(cli_t* cli, int argc, char **argv) {
    int c;

    while ((c = getopt_long(argc, argv, "zi:p:d:m:r:v:u:b:t:s:lh", opts, NULL)) != -1) {
        switch (c) {
            case 'z':
                cli->details = 1;

                break;

            case 'i':
                utils__str_copy(cli->host, optarg, sizeof(cli->host));

                break;

            case 'p':
                cli->port = atoi(optarg);

                break;

            case 'd':
                utils__str_copy(cli->domain, optarg, sizeof(cli->domain));

                break;

            case 'm':
                utils__str_copy(cli->method, optarg, sizeof(cli->method));

                break;

            case 'r':
                utils__str_copy(cli->path, optarg, sizeof(cli->path));

                break;

            case 'v':
                utils__str_copy(cli->http_version, optarg, sizeof(cli->http_version));

                break;

            case 'u':
                utils__str_copy(cli->ua, optarg, sizeof(cli->ua));

                break;

            case 'b':
                utils__str_copy(cli->body, optarg, sizeof(cli->body));

                break;

            case 't':
                cli->threads = atoi(optarg);

                break;

            case 's':
                cli->send_delay = strtoull(optarg, NULL, 0);

                break;

            case 'l':
                cli->list = 1;

                break;

            case 'h':
                cli->help = 1;

                break;

            case '?':
                fprintf(stderr, "Missing argument option...\n");

                break;

            default:
                break;
        }
    }
}