#include "simple_http_app.h"

// Static global variable for keeping loop running.
static bool run = true;

/**
 * Called on ctrl+C event. Sets global run variable to false.
 */
static void _int_handler(int32_t sig) {
    if (sig == SIGINT) {
        run = false;
        printf("\nShutting down\n");
    }
}

/**
 * Sets up server. This must be called before adding paths, starting server
 * and destroying server. It takes a constructed server (who's memory must
 * be managed by the caller), a path to a config file and the program's args.
 */
void config_server(Server* server, char* path, int32_t argc, char** argv) {
    if (!(run = signal(SIGINT, _int_handler) != SIG_ERR)) {
        perror("Error setting signal\n");
        return;
    }

    init_config(server, path, argc - 1, argv + 1);
    init_socket(server);
    init_poll(server);
    init_client_pool(server);

    init_default_responses(server);
    init_request(server);
    init_resposne(server);
    
    server->buffer = (char*)malloc(sizeof(char) * server->cfg->buffer_size);
    



    run = server->fd != -1;
}

/**
 * Starts server. Config must have been called before this function
 * is called.
 */
void start_server(Server* server) {
    if (run) {
        char ip_str[16];
        inet_ntop(AF_INET, &(server->cfg->ip), ip_str, INET_ADDRSTRLEN);
        ip_str[15] = '\0';
        fprintf(stdout, "Listening on http://%s:%hu (CTRL+C to quit)\n", ip_str, server->cfg->port);
        fflush(stdout);
    }

    // Server loop
    while(run) {

        if (has_event(server, &run) && run) {
            if (new_client_event(server)) {
                add_new_client(server);
            }
            for (int32_t i = 1; i < server->poll->fds_in_use && run; i++) {
                if (existing_client_event(i, server)) {
                    
                    int32_t fd = server->poll->fds[i].fd;
                    Client* client = g_hash_table_lookup(server->client_pool, &fd);
                    bool transfer_complete = false;

                    if (recv_from_client_successs(server, fd, client, &transfer_complete)) {
                        if (transfer_complete) {

                            if (!parse_request(server, client->raw_request)) {
                                send_default(server, fd, status_code.BAD_REQUEST);
                            }


                            //      => Check if path+meth exists
                            //      => call user method to construct response
                            //      => send response

                            else { send_default(server, fd, 200); } // <<---- remove 

                            restart_request(server);
                            restart_response(server);
                        }
                    } else {
                        remove_client_from_pool(server, i, fd);   
                    }
                }
            }
        }

        // Cleanup
        if (run) {
            timeout_clients(server);
            compress_fds(server);
        }
    }
}

/**
 * Releases all resources of the server. Note that it does not free 
 * the server it self. That responsibility falls on the caller.
 */
void destroy_server(Server* server) {

    destroy_config(server);

    destroy_socket(server);

    
    destroy_client_pool(server);

    // Destroy poll after client pool
    destroy_poll(server);

    destroy_default_responses(server);
    destroy_response(server);
    destroy_request(server);

    free(server->buffer);
}



/*
show fd array in loop...
        //##############################
        for (int i = 0; i < srv->cfg->max_clients + 1; i++) {
            if (srv->poll->fds[i].fd < 0) printf("## ");
            else if (srv->poll->fds[i].fd < 10) printf("0%d ", srv->poll->fds[i].fd);
            else printf("%d ", srv->poll->fds[i].fd);
        }
        putchar('\n');
        fflush(stdout);
        //##############################
*/