#include "socket.h"

/**
 * Sets up a web server socket.
 * 
 * @param sock_fd A pointer an integer that stores the socket FD when done.
 * @param bind_addr The bind IP address.
 * @param bind_port The bind port.
 * @param allow_reuse_port  If 1, sets the SO_REUSEPORT socket option to 1.
 * 
 * @return 0 on success. 1 on socket failure. 2 on bind address conversion failure. 3 on bind error. 4 on listen error.
 */
int server__socket_setup(int* sock_fd, const char* bind_addr, u16 bind_port, int allow_reuse_port) {
    int ret;

    // Create new socket.
    *sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (*sock_fd < 0)
        return 1;

    // Allow reuse of address.
    int opt = 1;

    setsockopt(*sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Allow reuse port if set.
    if (allow_reuse_port)
        setsockopt(*sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // Convert bind IP address to decimal format.
    struct in_addr in;

    if (inet_pton(AF_INET, bind_addr, &in) != 1) {
        close (*sock_fd);

        return 2;
    }

    // Generate socket information for binding.
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(bind_port);
    sin.sin_addr = in;

    // Bind socket.
    if (bind(*sock_fd, (struct sockaddr*)&sin, sizeof(sin)) != 0) {
        close(*sock_fd);

        return 3;
    }

    // Listen on socket.
    if (listen(*sock_fd, SOMAXCONN) != 0) {
        close(*sock_fd);

        return 4;
    }

    return 0;
}