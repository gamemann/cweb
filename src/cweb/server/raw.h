#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <cweb/http/common.h>
#include <cweb/http/request.h>
#include <cweb/http/response.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int server__setup_raw(ctx_t* ctx);