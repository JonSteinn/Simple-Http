#include "simple_http_server_prints.h"

void sh_print_shutdown() {
    printf("\nShutting down\n");
    fflush(stdout);
}

void sh_print_start_message(Server* server, bool run) {
    if (run) {
        char ip_str[16];
        inet_ntop(AF_INET, &(server->cfg->ip), ip_str, INET_ADDRSTRLEN);
        ip_str[15] = '\0';
        fprintf(stdout, "Listening on http://%s:%hu (CTRL+C to quit)\n", ip_str, server->cfg->port);
        fflush(stdout);
    }
}












/**
 * Display the entire configuration.
 */
void sh_print_config(Server* server) {
    if(!server->cfg->debug) { 
        return; 
    }

    Config* cfg = server->cfg;
    
    printf("\n##################################\nConfiguration:\n");

    printf("  * Debug mode: %s\n", cfg->debug ? "True" : "False");
    printf("  * Port: %hu\n", cfg->port);
    printf("  * Max queued: %d\n", cfg->max_queued);
    printf("  * Max clients: %d\n", cfg->max_clients);
    printf("  * Buffer size: %d\n", cfg->buffer_size);

    char ip_str[16];
    inet_ntop(AF_INET, &(cfg->ip), ip_str, INET_ADDRSTRLEN);
    printf("  * IP: %s\n", ip_str);

    printf("  * Poll timeout: %d\n", cfg->poll_timeout);
    printf("  * Inactive client timeout: %d\n", cfg->inactive_timeout);
    printf("  * Server name: %s\n", cfg->server_name);
    printf("  * Static cache time: %d\n", cfg->cache_time);
    
    printf("##################################\n\n");

    fflush(stdout);
}

/**
 * Display information on the initialized socket.
 */
void sh_print_socket(Server* server) {
    if (!server->cfg->debug) {
        return;
    }

    if (server->fd < 0) {
        printf("Error while initializing socket!\n");
    } else {
        printf("Socket's fd: %d\n", server->fd);
    }

    fflush(stdout);
}

void sh_print_config_done(Server* server, bool run) {
    if (!server->cfg->debug) {
        return;
    }

    printf("Configuring server complete: %s\n", run ? "Success" : "Failure");

    fflush(stdout);
}

void sh_print_adding_route(Server* server, const char* path, bool valid) {
    
    if (!server->cfg->debug) {
        return;
    }

    if (valid) {
        printf("Route added: %s\n", path);
    } else {
        printf("Invalid method, missing callback or invalid route: %s\n", path == NULL ? "<route missing>" : path);
    }

    fflush(stdout);
}

void sh_print_accept_failed(Server* server) {
    if (!server->cfg->debug) {
        return;
    }
        
    printf("Failed to accept connection\n");
    fflush(stdout);
}

void sh_print_no_room(Server* server) {
    if (!server->cfg->debug) {
        return;
    }
        
    printf("No room for a new client\n");
    fflush(stdout);
}

void sh_print_new_client(Server* server, Client* client) {
    if (!server->cfg->debug) {
        return;
    }

    printf("Client <%s:%hu> added\n", inet_ntoa(client->address.sin_addr), client->address.sin_port);
    fflush(stdout);
}

void sh_print_timeout(Server* server, int32_t fd) {
    if (!server->cfg->debug) {
        return;
    }

    Client* client = g_hash_table_lookup(server->client_pool, &fd);
    printf("Client <%s:%hu> timeout\n", inet_ntoa(client->address.sin_addr), client->address.sin_port);

    fflush(stdout);
}

void sh_print_remove_client(Server* server, int32_t fd) {
    if (!server->cfg->debug) {
        return;
    }

    Client* client = g_hash_table_lookup(server->client_pool, &fd);
    printf("Client <%s:%hu> removed\n", inet_ntoa(client->address.sin_addr), client->address.sin_port);

    fflush(stdout);
}

void sh_print_failed_to_recv(Server* server, Client* client) {
    if (!server->cfg->debug) {
        return;
    }
    printf("Failed to receive from client <%s:%hu>\n", inet_ntoa(client->address.sin_addr), client->address.sin_port);
    fflush(stdout);
}

void sh_print_parse_fail(Server* server, Client* client) {
    if (!server->cfg->debug) {
        return;
    }
    printf("Failed to parse request from client <%s:%hu>\n", inet_ntoa(client->address.sin_addr), client->address.sin_port);
    fflush(stdout);
}

void sh_print_request_response(Server* server, Client* client, bool found, bool head) {
    if (!server->cfg->debug) {
        return;
    }

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char timestamp[30];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, 30, "%a, %d %b %Y %X GMT", timeinfo);

    const char* method = head ? "HEAD" : 
                         server->request->method == METHOD_GET ? "GET" :
                         server->request->method == METHOD_POST ? "POST" : 
                         server->request->method == METHOD_PUT ? "PUT" :
                         server->request->method == METHOD_DELETE ? "DELETE" :
                         server->request->method == METHOD_CONNECTION ? "CONNECTION" :
                         server->request->method == METHOD_OPTIONS ? "OPTIONS" :
                         server->request->method == METHOD_PATCH ? "PATCH" :
                         "TRACE";

    const char* version = server->request->version == VERSION_10 ? "HTTP/1.0" :
                          server->request->version == VERSION_11 ? "HTTP/1.1" :
                          "HTTP/2.0";

    printf("> [%s]-[%s:%hu]-[%s]-[/%s]-[%s]-[%d]\n", 
           timestamp, 
           inet_ntoa(client->address.sin_addr), 
           client->address.sin_port,
           method,
           server->request->path->str,
           version,
           found ? server->response->status_code : 404);

    fflush(stdout);
}

void sh_print_fd_array(Server* server, bool before_compress) {
    if (!server->cfg->debug) {
        return;
    }
    printf(before_compress ? "FDs before: S" : "FDs after : S");
    for (int i = 1; i < server->poll->fds_in_use; i++) {
        putchar(server->poll->fds[i].fd < 0 ? '#' : 'C');
    }
    putchar('\n');
    fflush(stdout);
}







void sh_print_signal_error() {
    perror("Error setting signal\n");
    fflush(stderr);
}

void sh_print_socket_error() {
    perror("Failed to create socket!\n");
    fflush(stderr);
}

void sh_print_bind_error() {
    perror("Failed to bind socket!");
    fflush(stderr);
}

void sh_print_listen_error() {
    perror("Failed to listen on socket!\n");
    fflush(stderr);
}

void sh_print_poll_error(bool run) {
    if (run) {
        perror("Poll failed!\n");
        fflush(stderr);
    }
}