#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <cweb/http/common.h>
#include <cweb/http/request.h>
#include <cweb/http/response.h>
#include <cweb/fs/web.h>
#include <cweb/server/thread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/sysinfo.h>

int server__setup(ctx_t* ctx, int threads);
int server__shutdown(int threads);