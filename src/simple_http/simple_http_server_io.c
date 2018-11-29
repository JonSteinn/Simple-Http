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

int32_t send_default(server* srv, int32_t fd, int32_t status_code) {
    GString* res = (GString*)g_hash_table_lookup(srv->default_response, &status_code);
    if (res == NULL) {
        return 0;
    }
    return send_g_string(srv, fd, res);
}