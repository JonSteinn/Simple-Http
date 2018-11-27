#include "simple_http_server_poll.h"

/**
 * Create polling struct for server.
 */
void init_poll(server* srv) {
    srv->poll = (polling*)malloc(sizeof(polling));

    srv->poll->fds = (struct pollfd*)calloc(srv->cfg->max_clients + 1, sizeof(struct pollfd));
    for (int32_t i = 0; i <= srv->cfg->max_clients; i++) {
        srv->poll->fds[i].fd = -1;
        srv->poll->fds[i].events = POLLIN;
    }
    srv->poll->fds[0].fd = srv->fd;

    srv->poll->compress = false;
    srv->poll->fds_in_use = 1;

}

/**
 * Check if any communication is on the socket or if a 
 * waiting timeout occurs. This method only returns when
 * there is some request to process, if timeout occurs or
 * a system error while polling occurs. When an error occurs
 * then the run parameter is set to false and false is returned.
 * If a new request occurs we return true but false on timeout.
 */
bool has_event(server* srv, bool* run) {
    int32_t p = poll(srv->poll->fds, srv->poll->fds_in_use, srv->cfg->poll_timeout);
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
bool new_client_event(server* srv) {
    return existing_client_event(0, srv);
}

/**
 * Check if communication is an existing client number
 * index in the poll->fds array.
 */
bool existing_client_event(int32_t index, server* srv) {
    return srv->poll->fds[index].revents & POLLIN;
}

/**
 * Compress any removed fds so that clients always occupy
 * the array in indices 1,2,3,... (the sum of indices used
 * should always be the lowest possible).
 */
void compress_fds(server* srv) {
    if (srv->poll->compress) {
        _compress_fds_from_right_end(srv);
        _compress_fds_from_left_end(srv);
        _remove_excessive_fds(srv);    
        srv->poll->compress = false;
    }
}

/**
 * Reduce fds_in_use for any trailing fd that is -1.
 */
void _compress_fds_from_right_end(server* srv) {
    while (srv->poll->fds[srv->poll->fds_in_use - 1].fd == -1) {
        srv->poll->fds_in_use--;
    }
}

/**
 * For any -1 in the middle of the array (e.g. excluding ind=0 and the last), we move
 * each of the higher indices one back. 
 */
void _compress_fds_from_left_end(server* srv) {
    for (int32_t i = 1; i < srv->poll->fds_in_use - 1; i++) {
        // TODO: Find run of -1's and move by run for each or even beter, memcopy it....
        if (srv->poll->fds[i].fd == -1) {
            for(int32_t j = i; j < srv->poll->fds_in_use - 1; j++) {
                srv->poll->fds[j].fd = srv->poll->fds[j+1].fd;
                srv->poll->fds[j].revents = srv->poll->fds[j+1].revents;
                srv->poll->fds[j].events = srv->poll->fds[j+1].events;
            }
            srv->poll->fds_in_use--;
        }
    }
}

/**
 * If any of the moved indices left a trail of non -1 fds at the
 * right end, they are set to -1 here.
 */
void _remove_excessive_fds(server* srv) {
    for (int32_t i = srv->poll->fds_in_use; i < srv->cfg->max_clients + 1 && srv->poll->fds[i].fd != -1; i++) {
        srv->poll->fds[i].fd = -1;
    }
}

/**
 * Realease resources for polling struct. It handles
 * freeing any deep memory allocated by the initializer.
 */
void destroy_poll(server* srv) {
    free(srv->poll->fds);
    free(srv->poll);
}
