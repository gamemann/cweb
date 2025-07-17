#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/error/error.h>

#include <utils/http/common.h>
#include <utils/http/request.h>
#include <utils/http/response.h>

#include <cweb/ctx/ctx.h>

#include <cweb/fs/web.h>
#include <cweb/server/socket.h>
#include <cweb/logger/logger.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>

struct thread_ctx {
    int id;
    
    ctx_t* ctx;
    int global_sock_fd;
} typedef thread_ctx_t;

void* server__thread(void* ctx);