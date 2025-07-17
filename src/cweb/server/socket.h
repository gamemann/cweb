#pragma once

#include <utils/constants.h>
#include <utils/int_types.h>

#include <utils/error/error.h>

#include <cweb/fs/web.h>
#include <cweb/server/thread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>

int server__socket_setup(int* sock_fd, const char* bind_addr, u16 bind_port, int allow_reuse_port);