#include "simple_http_server_socket.h"

/**
 * Initialize the socket for the server.
 */
void init_socket(server* srv) {
    _initialize_address(srv);
    _setup_socket(srv);
    _bind_socket(srv);
    _listen_on_socket(srv);
    if (srv->cfg->debug) {
        _socket_display(srv);
    }
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
    srv->fd = socket(AF_INET, SOCK_STREAM, 0);
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
    if (srv->fd == -1) {
        return;
    }

    // If port is in use, we try the next port up to 5 tries.
    int32_t tries = 10;
    while(true) {
        int32_t bind_results = bind(srv->fd, (struct sockaddr*)&srv->address, (socklen_t)sizeof(struct sockaddr_in));
        if (bind_results >= 0) {
            break;
        } else if (errno == EADDRINUSE) {
            tries--;
            if (tries == 0) {
                srv->fd = -1;
                break;
            }
            srv->cfg->port = (srv->cfg->port + 1) < 1024 ? 1024 : srv->cfg->port + 1;
            srv->address.sin_port = htons(srv->cfg->port);
        } else {
            srv->fd = -1;
            break;
        }
    }

    if (srv->fd < 0) {
        perror("Failed to bind socket!");
    }
}

/**
 * Attempt to set our socket as a passive one for other incoming connections.
 * If unsuccessful, we set the server's fd to -1.
 */
void _listen_on_socket(server* srv) {
    if (srv->fd > 0 && listen(srv->fd, srv->cfg->max_queued) < 0) {
        perror("Failed to listen on socket!\n");
        srv->fd = -1;
    }
}

/**
 * Display information on the initialized socket.
 */
void _socket_display(server* srv) {
    if (srv->fd < 0) {
        printf("Error while initializing socket!\n");
    } else {
        printf("Socket's fd = %d\n", srv->fd);
    }
}

/**
 * Deep cleanup for socket related variables for server.
 */
void destroy_socket(server* srv) {
    if (srv->fd != -1) {
        close(srv->fd);
    }
}
