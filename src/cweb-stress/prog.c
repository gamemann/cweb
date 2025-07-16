#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/sysinfo.h>

#include <signal.h>

#include <cweb-stress/cli/cli.h>
#include <cweb-stress/client/thread.h>

pthread_t pthreads[MAX_THREADS];

int cont = 1;

void sig_hdl(int code) {
    cont = 0;
}

int main(int argc, char** argv) {
    // Initialize CLI.
    cli_t cli = {0};

    cli__parse(&cli, argc, argv);

    // Help menu.
    if (cli.help) {

        return EXIT_SUCCESS;
    }

    // Check host length.
    if (strlen(cli.host) < 1) {
        fprintf(stderr, "No host specified. Please set --host or -i argument.\n");

        return EXIT_FAILURE;
    }

    // Check port length.
    if (cli.port < 1) {
        fprintf(stderr, "Invalid port. Please make sure you've set the port via --port or -p argument.\n");

        return EXIT_FAILURE;
    }

    // Retrieve thread count automatically if below 1.
    if (cli.threads < 1)
        cli.threads = get_nprocs();

    // Make sure we have a domain set.
    if (strlen(cli.domain) < 1)
        strcpy(cli.domain, "localhost");

    // Make sure method is set.
    if (strlen(cli.method) < 1)
        strcpy(cli.method, "GET");

    // Make sure path is set.
    if (strlen(cli.path) < 1)
        strcpy(cli.path, "/");

    // Make sure HTTP version is set.
    if (strlen(cli.http_version) < 1)
        strcpy(cli.http_version, "HTTP/1.1");

    // Check for list.
    if (cli.list) {
        printf("Current Settings:\n");

        printf("Host: %s\n", cli.host);
        printf("Port: %d\n", cli.port);

        printf("Domain: %s\n", cli.domain);

        printf("Method: %s\n", cli.method);
        printf("Path: %s\n", cli.path);
        printf("HTTP Version: %s\n", cli.http_version);

        printf("UA: %s\n", cli.ua);

        printf("Threads: %d\n", cli.threads);
        printf("Send Delay: %llu\n", cli.send_delay);

        if (strlen(cli.body) > 0) {
            printf("\n\n");

            printf("Body:\n%s\n", cli.body);
        }
    
        return EXIT_SUCCESS;
    }

    printf("Sending to %s:%d.. Domain=%s Method=%s Path=%s HTTP_Version=%s UA=%s Threads=%d Send_Delay=%llu\n", cli.host, cli.port, cli.domain, cli.method, cli.path, cli.http_version, cli.ua, cli.threads, cli.send_delay);

    // Create threads.
    for (int i = 0; i < cli.threads; i++) {
        // Allocate thread context.
        thread_ctx_t *tctx = malloc(sizeof(thread_ctx_t));

        if (!tctx) {
            fprintf(stderr, "Failed to create thread #%d due to failed allocation of thread context.\n", i + 1);

            continue;
        }

        // Assign thread context info.
        tctx->id = i + 1;

        // We're copying all values to a new context so only one thread can touch its memory.
        strcpy(tctx->host, cli.host);
        tctx->port = cli.port;

        strcpy(tctx->domain, cli.domain);

        strcpy(tctx->method, cli.method);
        strcpy(tctx->path, cli.path);
        strcpy(tctx->http_version, cli.http_version);

        strcpy(tctx->ua, cli.ua);

        strcpy(tctx->body, cli.body);

        tctx->send_delay = cli.send_delay;

        // Create thread.
        pthread_create(&pthreads[i], NULL, client__thread_main, (void*)tctx);
    }

    // Create signals.
    signal(SIGINT, sig_hdl);
    signal(SIGTERM, sig_hdl);

    // Create loop (runs every second to prevent program from shutting down until signal is caught).
    while (cont) {
        sleep(1);
    }

    printf("Shutting down... Bye!\n");

    return EXIT_SUCCESS;
}