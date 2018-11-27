#include "simple_http_client_handler.h"

void init_client_pool(server* srv) {
    srv->client_pool = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, _free_client);
}

void _free_client(gpointer mem) {
    client* cli = (client*)mem;
    if (cli->raw_request != NULL) {
        g_string_free(cli->raw_request, true);
    }
    free(cli);
}

void add_new_client(server* srv) {
    if (srv->poll->fds_in_use == srv->cfg->max_clients + 1) {
        // TODO: DUMP CLIENT because no room
        fprintf(stdout, "NO ROOM!\n"); fflush(stdout); // TODO: REMOVE
        return;
    }
    
    client* new_client = _construct_new_client();
    int32_t fd = _accept_connection(srv, new_client);
    if (fd >= 0) {
        _add_client_to_pool(srv, new_client, fd);
    }
}

client* _construct_new_client() {
    client* new_client = (client*)malloc(sizeof(client));
    new_client->last_active = time(NULL);
    memset(&new_client->address, 0, sizeof(struct sockaddr_in));
    new_client->raw_request = NULL;
    return new_client;
}

int32_t _accept_connection(server* srv, client* new_client) {
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
    int32_t fd = accept(srv->fd, (struct sockaddr*)&new_client->address, &len);
    if (fd < 0) {
        _free_client((gpointer)new_client);
        if (srv->cfg->debug) {
            fprintf(stdout, "Fail to accept connection!\n");
            fflush(stdout);   
        }
        return -1;
    }

    return fd;
}

void _add_client_to_pool(server* srv, client* new_client, int32_t fd) {
    srv->poll->fds[srv->poll->fds_in_use].fd = fd;
    srv->poll->fds_in_use++;
    g_hash_table_insert(srv->client_pool, alloc_int(fd), new_client);
}

void handle_client(server* srv, int32_t index) {
    int fd = srv->poll->fds[index].fd;
    client* cli = g_hash_table_lookup(srv->client_pool, &fd);
    if (cli == NULL) {
        // TODO: handle not found
        perror("CLIENT NOT FOUND!");
        exit(1);
    }
    
    if (cli->raw_request == NULL) {
        cli->raw_request = g_string_new_len(NULL, srv->cfg->buffer_size);
    }

    // TODO: replace with timeout mechanic wrapped around recv
    ssize_t received = recv(fd, srv->buffer, srv->cfg->buffer_size - 1, 0);

    if (received <= 0) {
        perror("error from recv\n");
        exit(1);
        //TODO: handle better and handle =0 seperately
    }

    srv->buffer[received] = '\0';
    g_string_append(cli->raw_request, srv->buffer);

    bool lastData = recv(fd, srv->buffer, 1, MSG_PEEK | MSG_DONTWAIT) != 1;
    if (lastData) {
        printf("%s\n", cli->raw_request->str);
        g_string_truncate(cli->raw_request, 0);
        send(fd, "HTTP/1.1 200 Ok\r\nContent-Type: text/html\r\nContent-Length: 56\r\n\r\n<html><head><title>A</title></head><body>B</body></html>", 128, 0);
    }
}

void timeout_clients(server* srv) {
    for (int32_t i = 1; i < srv->poll->fds_in_use; i++) {
        int32_t fd = srv->poll->fds[i].fd;
        if (fd >= 0) {
            time_t now = time(NULL);
            time_t last = ((client*)g_hash_table_lookup(srv->client_pool, &fd))->last_active;

            if (difftime(now, last) >= srv->cfg->inactive_timeout) {
                _remove_client_from_pool(srv, i, fd);
            }
        }
    }
    
}

void _remove_client_from_pool(server* srv, int32_t i, int32_t fd) {
    g_hash_table_remove(srv->client_pool, &fd);
    close(fd);
    srv->poll->fds[i].fd = -1;
    srv->poll->fds[i].revents = 0;
    srv->poll->compress = true;
}

void destroy_client_pool(server* srv) {
    g_hash_table_destroy(srv->client_pool);
    for (int32_t i = srv->poll->fds_in_use - 1; i > 0; i--) {
        close(srv->poll->fds[i].fd);
    }
}