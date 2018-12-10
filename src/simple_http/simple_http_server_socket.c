#include "simple_http_server_socket.h"

/**
 * Initialize the socket for the server.
 */
void init_socket(Server* server, bool* run) {
    _initialize_address(server);
    _setup_socket(server);
    _bind_socket(server);
    _listen_on_socket(server);
    if (server->fd == -1) {
        *run = false;
    }
    
    sh_print_socket(server);
}

/**
 * Set the address variable (sockaddr_in) to
 *  - IPv4 protocolOwn IP address
 *  - Chosen port (default: 8080)
 *  - Chosen IP (default: INADDR_ANY)
 */
void _initialize_address(Server* server) {
    memset(&server->address, 0, sizeof(struct sockaddr_in));
    server->address.sin_family = AF_INET;
    server->address.sin_port = htons(server->cfg->port);
    server->address.sin_addr.s_addr = htonl(INADDR_ANY);
}

/**
 * Attempt to create endpoint. Sets descriptor to the
 * one for the socket if successful, -1 otherwise.
 */
void _setup_socket(Server* server) {
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        sh_print_socket_error();
        server->fd = -1;
    }
}

/**
 * Attempt to assign the address to the socket. If unsuccessful
 * then the server's fd is set to -1.
 */
void _bind_socket(Server* server) {
    if (server->fd == -1) {
        return;
    }

    // If port is in use, we try the next port up to 5 tries.
    int32_t tries = 10;
    while(true) {
        int32_t bind_results = bind(server->fd, (struct sockaddr*)&server->address, (socklen_t)sizeof(struct sockaddr_in));
        
        if (bind_results >= 0) {
            // Success
            break;
        } else if (errno == EADDRINUSE) {
            // Socket in use
            tries--;
            if (tries == 0) {
                server->fd = -1;
                break;
            }
            server->cfg->port = server->cfg->port == 65535 ? 1024 : server->cfg->port + 1;
            server->address.sin_port = htons(server->cfg->port);
        } else {
            // Error other then socket in use
            server->fd = -1;
            break;
        }
    }

    if (server->fd < 0) {
        sh_print_bind_error();
    }
}

/**
 * Attempt to set our socket as a passive one for other incoming connections.
 * If unsuccessful, we set the server's fd to -1.
 */
void _listen_on_socket(Server* server) {
    if (server->fd > 0 && listen(server->fd, server->cfg->max_queued) < 0) {
        sh_print_listen_error();
        server->fd = -1;
    }
}

/**
 * Deep cleanup for socket related variables for server.
 */
void destroy_socket(Server* server) {
    if (server->fd != -1) {
        close(server->fd);
    }
}
