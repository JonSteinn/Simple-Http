#include "simple_http_server_io.h"

int32_t send_g_string(server* srv, int32_t fd, GString* response) {
    size_t total = 0;
    
    while (total < response->len) {
        ssize_t tmp = send(fd, response->str + total, response->len - total, 0);
        if (tmp < 0) {
            if (srv->cfg->debug) {
                perror("Failed to send\n");
            }
            return 0;
        }
        total += tmp;
    }

    return 1;
}