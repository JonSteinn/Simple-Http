#include "simple_http_server_io.h"

void init_io(Server* server) {    
    server->buffer = (char*)malloc(sizeof(char) * server->cfg->buffer_size);
}

void destroy_io(Server* server) {
    free(server->buffer);
}

int32_t send_g_string(Server* server, int32_t fd, GString* response) {
    size_t total = 0;
    
    while (total < response->len) {
        ssize_t tmp = send(fd, response->str + total, response->len - total, 0);
        if (tmp < 0) {
            if (server->cfg->debug) {
                perror("Failed to send\n");
            }
            return 0;
        }
        total += tmp;
    }

    return 1;
}

int32_t send_default(Server* server, int32_t fd, int32_t status_code) {
    GString* res = (GString*)g_hash_table_lookup(server->default_response, &status_code);
    if (res == NULL) {
        return 0;
    }
    return send_g_string(server, fd, res);
}

bool recv_from_client_successs(Server* server, int32_t fd, Client* client, bool* complete) {
    ssize_t received = recv(fd, server->buffer, server->cfg->buffer_size - 1, 0);
    if (received <= 0) {
        return false;
    }
    server->buffer[received] = '\0';
    g_string_append(client->raw_request, server->buffer);
    *complete = received < server->cfg->buffer_size - 1 || 
                recv(fd, server->buffer, 1, MSG_PEEK | MSG_DONTWAIT) != 1;
    return true;
}
