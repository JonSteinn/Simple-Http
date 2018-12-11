#include "simple_http_server_io.h"

/**
 * Initialize buffer to read from sockets.
 */
void init_io(Server* server) {    
    server->buffer = (char*)malloc(sizeof(char) * server->cfg->buffer_size);
}

/**
 * Free buffer.
 */
void destroy_io(Server* server) {
    free(server->buffer);
}

/**
 * Send a response as a GString* to the client with fd. If successful,
 * true is returned, otherwise false.
 */
bool send_g_string(Server* server, int32_t fd, GString* response) {
    size_t total = 0;
    // Since send does not guarantee to send the entire 
    // message, se need to keep sending until done.
    while (total < response->len) {
        ssize_t tmp = send(fd, response->str + total, response->len - total, 0);
        if (tmp < 0) {
            sh_print_send_error(server);
            return false;
        }
        total += tmp;
    }
    return true;
}

/**
 * Send the default repsonse for a given status code to the 
 * client with fd. Returns true iff succesful.
 */
bool send_default(Server* server, int32_t fd, int32_t status_code) {
    GString* res = (GString*)g_hash_table_lookup(server->default_response, &status_code);
    if (res == NULL) {
        return false;
    }
    return send_g_string(server, fd, res);
}

/**
 * Read from the socket of client with fd. If successful, true is returned,
 * otherwise false. If there is more to be read from the socket, the complete
 * parameter is set to false, otherwise true. Each successful read is appended
 * to the client's raw request string which should therefore contain the entire
 * request when completed.
 */
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
