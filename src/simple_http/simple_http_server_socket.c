#include "simple_http_server_socket.h"

/**
 * Initialize the socket for the server.
 */
void init_socket(server* srv) {
    _initialize_address(srv);
    _setup_socket(srv);
    _bind_socket(srv);
    _listen_on_socket(srv);
    if (srv->cfg->debug) _socket_display(srv);
}

/**
 * Set the address variable (sockaddr_in) to
 *  - IPv4 protocolOwn IP address
 *  - Chosen port (default: 8080)
 *  - Chosen IP (default: INADDR_ANY)
 */
void _initialize_address(server* srv) {
    memset(&srv->address, 0, sizeof(struct sockaddr_in));
    srv->address.sin_family = AF_INET;
    srv->address.sin_port = htons(srv->cfg->port);
    srv->address.sin_addr.s_addr = htonl(INADDR_ANY);
}

/**
 * Attempt to create endpoint. Sets descriptor to the
 * one for the socket if successful, -1 otherwise.
 */
void _setup_socket(server* srv) {
    srv->fd = socket(AF_INET, SOCK_STREAM/*TCP*/, 0);
    if (srv->fd < 0) {
        perror("Failed to create socket!\n");
        srv->fd = -1;
    }
}

/**
 * Attempt to assign the address to the socket. If unsuccessful
 * then the server's fd is set to -1.
 */
void _bind_socket(server* srv) {
    if (srv->fd < 0 || bind(srv->fd, (struct sockaddr*)&srv->address, (socklen_t)sizeof(struct sockaddr_in)) < 0) {
        if (srv->fd >= 0) perror("Failed to bind socket!\n");
        srv->fd = -1;
    }
}

/**
 * Attempt to set our socket as a passive one for other incoming connections.
 * If unsuccessful, we set the server's fd to -1.
 */
void _listen_on_socket(server* srv) {
    if (srv->fd < 0 || listen(srv->fd, srv->cfg->max_queued) < 0) {
        if (srv->fd >= 0) perror("Failed to listen on socket!\n");
        srv->fd = -1;
    }
}

/**
 * Display information on the initialized socket.
 */
void _socket_display(server* srv) {
    if (srv->fd < 0) {
        fprintf(stdout, "Error while initializing socket!\n");
    } else {
        fprintf(stdout, "Socket's fd = %d\n", srv->fd);
    }
    fflush(stdout);
}

/**
 * Deep cleanup for socket related variables for server.
 */
void destroy_socket(server* srv) {
    if (srv->fd != -1) close(srv->fd);
}
