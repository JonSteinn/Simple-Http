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
void config_server(server* srv, char* path, int32_t argc, char** argv) {
    if (!(run = signal(SIGINT, _int_handler) != SIG_ERR)) {
        perror("Error setting signal\n");
        return;
    }

    init_config(srv, path, argc - 1, argv + 1);
    init_socket(srv);
    init_poll(srv);
    init_client_pool(srv);

    init_default_responses(srv);
    
    srv->buffer = (char*)malloc(sizeof(char) * srv->cfg->buffer_size);
    



    run = srv->fd != -1;
}

/**
 * Starts server. Config must have been called before this function
 * is called.
 */
void start_server(server* srv) {
    if (run) {
        char ip_str[16];
        inet_ntop(AF_INET, &(srv->cfg->ip), ip_str, INET_ADDRSTRLEN);
        ip_str[15] = '\0';
        fprintf(stdout, "Listening on http://%s:%hu (CTRL+C to quit)\n", ip_str, srv->cfg->port);
        fflush(stdout);
    }

    // Server loop
    while(run) {

        if (has_event(srv, &run) && run) {
            if (new_client_event(srv)) {
                add_new_client(srv);
            }
            for (int32_t i = 1; i < srv->poll->fds_in_use && run; i++) {
                if (existing_client_event(i, srv)) {
                    handle_client(srv, i); // <--- don't keep in client handler

                    // RECV next buffer size at most
                    //
                    // Then
                    // 
                    // If error or 0-term from client 
                    //      => remove and close
                    // Else if Done
                    //      => Parse request
                    //      => Check if path+meth exists
                    //      => call user method to construct response
                    //      => send response
                    // ELSE
                    //      => Do nothing
                }
            }
        }

        // Cleanup
        if (run) {
            timeout_clients(srv);
            compress_fds(srv);
        }
    }
}

/**
 * Releases all resources of the server. Note that it does not free 
 * the server it self. That responsibility falls on the caller.
 */
void destroy_server(server* srv) {

    destroy_config(srv);

    destroy_socket(srv);

    
    destroy_client_pool(srv);

    // Destroy poll after client pool
    destroy_poll(srv);

    destroy_default_responses(srv);

    free(srv->buffer);
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