#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/http/common.h>
#include <utils/http/request.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <pthread.h>

#include <errno.h>

#define ALLOW_THREAD_DELAY

#define ALLOW_DETAILS

struct thread_ctx {
    int id;

    int details;

    char host[HTTP_HOST_MAX_LEN];
    u16 port;

    char domain[HTTP_DOMAIN_MAX_LEN];

    char method[HTTP_METHOD_MAX_LEN];
    char path[HTTP_PATH_MAX_LEN];
    char http_version[HTTP_VERSION_MAX_LEN];

    char ua[HTTP_UA_MAX_LEN];

    char body[4096];

    u64 send_delay;
} typedef thread_ctx_t;

void* client__thread_main(void* tmp);