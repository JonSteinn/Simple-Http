#include "simple_http_client_handler.h"

// Defines
#define CLIENT_START_INDEX 1
#define NO_FD -1
#define NO_REVENTS 0
#define ERROR -1

// 'Private' function definitions
void _free_client(gpointer mem);
Client* _construct_new_client(Server* srv);
int32_t _accept_connection(Server* srv, Client* client);
void _add_client_to_pool(Server* srv, Client* client, int32_t fd);
void _answer_client(Server* server, Client* client, int32_t fd, int32_t i);

/**
 * Initialize the client pool. That is a dictionary that maps a
 * file descriptor to a client data object.
 */
void init_client_pool(Server* server) {
    server->client_pool = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, _free_client);
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
        sh_print_no_room(server);
        send_default(server, fd, status_code.SERVICE_UNAVAILABLE);
        _free_client((gpointer)new_client);
        close(fd);
    } else {
        _add_client_to_pool(server, new_client, fd);
    }
}

/**
 * Check if any of the clients have been inactive for timeout limit,
 * and if so - remove them.
 */
void timeout_clients(Server* server) {
    for (int32_t i = CLIENT_START_INDEX; i < server->poll->fds_in_use; i++) {
        int32_t fd = server->poll->fds[i].fd;
        if (fd >= 0) {
            time_t now = time(NULL);
            time_t last = ((Client*)g_hash_table_lookup(server->client_pool, &fd))->last_active;
            if (difftime(now, last) >= server->cfg->inactive_timeout) {
                sh_print_timeout(server, fd);
                remove_client_from_pool(server, i, fd);
            }
        }
    }
    
}

/**
 * Remove client from the client pool and close its socket.
 */
void remove_client_from_pool(Server* server, int32_t index, int32_t fd) {
    sh_print_remove_client(server, fd);
    g_hash_table_remove(server->client_pool, &fd);
    close(fd);
    server->poll->fds[index].fd = NO_FD;
    server->poll->fds[index].revents = NO_REVENTS;
    server->poll->compress = true;
}

/**
 * Process whatever the client is requesting.
 */
void proccess_client_event(Server* server, int32_t i) {
    // Extract needed info
    int32_t fd = server->poll->fds[i].fd;
    Client* client = g_hash_table_lookup(server->client_pool, &fd);
    bool transfer_complete = false;

    // Restart timeout
    client->last_active = time(NULL);

    if (recv_from_client_successs(server, fd, client, &transfer_complete)) {
        if (transfer_complete) {
            _answer_client(server, client, fd, i);
        }
    } else {
        sh_print_failed_to_recv(server, client);
        remove_client_from_pool(server, i, fd);
    }
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

/////////////////////
// Private helpers //
/////////////////////

/**
 * Free mechanic for a client in a dictionary.
 */
void _free_client(gpointer mem) {
    Client* client = (Client*)mem;
    g_string_free(client->raw_request, true);
    free(client);
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
        sh_print_accept_failed(server);
        return ERROR;
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
    sh_print_new_client(server, client);
}

/**
 * When client has stopped requesting and is ready for
 * a response, that response is created here based on
 * the request.
 */
void _answer_client(Server* server, Client* client, int32_t fd, int32_t i) {
    if (!parse_request(server, client->raw_request)) {
        sh_print_parse_fail(server, client);
        send_default(server, fd, status_code.BAD_REQUEST);
        remove_client_from_pool(server, i, fd);
    } else {
        // Convert head to get
        bool head = server->request->method == METHOD_HEAD;
        if (head) {
            server->request->method = METHOD_GET;
        }
        
        bool found = find_and_call_route_callback(server) || 
                     (server->static_files->has_static && 
                     server->request->method == METHOD_GET && 
                     read_file_into_response(server));

        if (found) {
            set_default_response_headers(server);
            GString* response = convert_response_to_string(server, head);
            send_g_string(server, fd, response);
            g_string_free(response, true);
        } else {
            send_default(server, fd, status_code.NOT_FOUND);
        }

        sh_print_request_response(server, client, found, head);
        log_request(server, client, found, head);

        if (!keep_alive(server)) {
            remove_client_from_pool(server, i, fd);
        }
    }

    restart_request(server);
    restart_response(server);
}

// Remove defines
#undef CLIENT_START_INDEX
#undef NO_FD
#undef NO_REVENTS
#undef ERROR