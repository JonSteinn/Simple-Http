#include "simple_http_client_handler.h"

/**
 * Initialize the client pool. That is a dictionary that maps a
 * file descriptor to a client data object.
 */
void init_client_pool(Server* server) {
    server->client_pool = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, _free_client);
}

/**
 * Free mechanic for a client in a dictionary.
 */
void _free_client(gpointer mem) {
    Client* client = (Client*)mem;
    g_string_free(client->raw_request, true);
    free(client);
}

/**
 * Add a new client to the server's client pool, given that
 * some client is trying to connect. If there is no room,
 * we accept his connection but send a default server
 * unavailable response.
 */
void add_new_client(Server* server) {
    Client* new_client = _construct_new_client(server);
    int32_t fd = _accept_connection(server, new_client);
    
    if (fd < 0) {
        return;
    }

    if (server->poll->fds_in_use == server->cfg->max_clients + 1) {
        //SH_DEBUG(server, "No room for a new client\n");
        send_default(server, fd, status_code.SERVICE_UNAVAILABLE);
        _free_client((gpointer)new_client);
        close(fd);
    } else {
        _add_client_to_pool(server, new_client, fd);
    }
}

/**
 * Allocate a client object which is returned.
 */
Client* _construct_new_client(Server* server) {
    Client* new_client = (Client*)malloc(sizeof(Client));
    new_client->last_active = time(NULL);
    memset(&new_client->address, 0, sizeof(struct sockaddr_in));
    new_client->raw_request = g_string_new_len(NULL, server->cfg->buffer_size);
    return new_client;
}

/**
 * Try to establish a connection with a client (TCP handshake).
 * If successful, the file descriptor is returned. Otherwise
 * -1 is returned.
 */
int32_t _accept_connection(Server* server, Client* client) {
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
    int32_t fd = accept(server->fd, (struct sockaddr*)&client->address, &len);
    if (fd < 0) {
        _free_client((gpointer)client);
        //SH_DEBUG(server, "Failed to accept connection\n");
        return -1;
    }
    return fd;
}

/**
 * Add a client to the client pool.
 */
void _add_client_to_pool(Server* server, Client* client, int32_t fd) {
    server->poll->fds[server->poll->fds_in_use].fd = fd;
    server->poll->fds_in_use++;
    g_hash_table_insert(server->client_pool, alloc_int(fd), client);
}

/**
 * Check if any of the clients have been inactive for timeout limit,
 * and if so - remove them.
 */
void timeout_clients(Server* server) {
    for (int32_t i = 1; i < server->poll->fds_in_use; i++) {
        int32_t fd = server->poll->fds[i].fd;
        if (fd >= 0) {
            time_t now = time(NULL);
            time_t last = ((Client*)g_hash_table_lookup(server->client_pool, &fd))->last_active;

            if (difftime(now, last) >= server->cfg->inactive_timeout) {
                remove_client_from_pool(server, i, fd);
            }
        }
    }
    
}

/**
 * Remove client from the client pool and close its socket.
 */
void remove_client_from_pool(Server* server, int32_t index, int32_t fd) {
    g_hash_table_remove(server->client_pool, &fd);
    close(fd);
    server->poll->fds[index].fd = -1;
    server->poll->fds[index].revents = 0;
    server->poll->compress = true;
}

/**
 * Clean up all sockets (except the server's) and clients.
 */
void destroy_client_pool(Server* server) {
    g_hash_table_destroy(server->client_pool);
    for (int32_t i = server->poll->fds_in_use - 1; i > 0; i--) {
        close(server->poll->fds[i].fd);
    }
}
