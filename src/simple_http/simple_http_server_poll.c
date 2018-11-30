#include "simple_http_server_poll.h"

/**
 * Create polling struct for server.
 */
void init_poll(Server* server) {
    server->poll = (Polling*)malloc(sizeof(Polling));

    server->poll->fds = (struct pollfd*)calloc(server->cfg->max_clients + 1, sizeof(struct pollfd));
    for (int32_t i = 0; i <= server->cfg->max_clients; i++) {
        server->poll->fds[i].fd = -1;
        server->poll->fds[i].events = POLLIN;
    }
    server->poll->fds[0].fd = server->fd;

    server->poll->compress = false;
    server->poll->fds_in_use = 1;

}

/**
 * Check if any communication is on the socket or if a 
 * waiting timeout occurs. This method only returns when
 * there is some request to process, if timeout occurs or
 * a system error while polling occurs. When an error occurs
 * then the run parameter is set to false and false is returned.
 * If a new request occurs we return true but false on timeout.
 */
bool has_event(Server* server, bool* run) {
    int32_t p = poll(server->poll->fds, server->poll->fds_in_use, server->cfg->poll_timeout);
    if (p < 0) {
        if (*run) {
            perror("Poll failed!\n");
        }
        return (*run = false);
    }
    return p != 0;
}

/**
 * Check if communication is a new client.
 */
bool new_client_event(Server* server) {
    return existing_client_event(0, server);
}

/**
 * Check if communication is an existing client number
 * index in the poll->fds array.
 */
bool existing_client_event(int32_t index, Server* server) {
    return server->poll->fds[index].revents & POLLIN;
}

/**
 * Compress any removed fds so that clients always occupy
 * the array in indices 1,2,3,... (the sum of indices used
 * should always be the lowest possible).
 */
void compress_fds(Server* server) {
    if (server->poll->compress) {
        // TODO: Improve this algorithms
        /* Two pointers for copying would improve this */
        _compress_fds_from_right_end(server);
        _compress_fds_from_left_end(server);
        _remove_excessive_fds(server);    
        server->poll->compress = false;
    }
}

/**
 * Reduce fds_in_use for any trailing fd that is -1.
 */
void _compress_fds_from_right_end(Server* server) {
    while (server->poll->fds[server->poll->fds_in_use - 1].fd == -1) {
        server->poll->fds_in_use--;
    }
}

/**
 * For any -1 in the middle of the array (e.g. excluding ind=0 and the last), we move
 * each of the higher indices one back. 
 */
void _compress_fds_from_left_end(Server* server) {
    for (int32_t i = 1; i < server->poll->fds_in_use - 1; i++) {
        // TODO: Find run of -1's and move by run for each or even beter, memcopy it....
        if (server->poll->fds[i].fd == -1) {
            for(int32_t j = i; j < server->poll->fds_in_use - 1; j++) {
                server->poll->fds[j].fd = server->poll->fds[j+1].fd;
                server->poll->fds[j].revents = server->poll->fds[j+1].revents;
                server->poll->fds[j].events = server->poll->fds[j+1].events;
            }
            server->poll->fds_in_use--;
        }
    }
}

/**
 * If any of the moved indices left a trail of non -1 fds at the
 * right end, they are set to -1 here.
 */
void _remove_excessive_fds(Server* server) {
    for (int32_t i = server->poll->fds_in_use; i < server->cfg->max_clients + 1 && server->poll->fds[i].fd != -1; i++) {
        server->poll->fds[i].fd = -1;
    }
}

/**
 * Realease resources for polling struct. It handles
 * freeing any deep memory allocated by the initializer.
 */
void destroy_poll(Server* server) {
    free(server->poll->fds);
    free(server->poll);
}
