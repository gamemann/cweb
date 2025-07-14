#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <cweb/http/common.h>
#include <cweb/http/request.h>
#include <cweb/http/response.h>
#include <cweb/fs/web.h>

#include <cweb/ctx/ctx.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>

struct thread_ctx {
    int id;
    
    ctx_t* ctx;
    int sock_fd;
} typedef thread_ctx_t;

void* server__thread(void* ctx);