#include "simple_http_app.h"

static bool run = true;
static void _int_handler(int32_t sig) {
    if (sig == SIGINT) run = false;
    putchar('\n');
}


void config_server(server* srv, char* path, int32_t argc, char** argv) {
    if (!(run = signal(SIGINT, _int_handler) != SIG_ERR)) {
        perror("Error setting signal\n");
        run = false;
        return;
    }

    init_config(srv, path, argc - 1, argv + 1);
    init_socket(srv);
    init_poll(srv);
    init_client_pool(srv);
    
    srv->buffer = (char*)malloc(sizeof(char) * srv->cfg->buffer_size);
    


    run = srv->fd != -1;
}

void start_server(server* srv) {
    if (srv->cfg->debug) {
        char ip_str[16];
        inet_ntop(AF_INET, &(srv->cfg->ip), ip_str, INET_ADDRSTRLEN);
        fprintf(stdout, "Listening on http://%s:%hu (CTRL+C to quit)\n", ip_str, srv->cfg->port);
        fflush(stdout);
    }

    while(run) {

        //##############################
        for (int i = 0; i < srv->cfg->max_clients + 1; i++) {
            if (srv->poll->fds[i].fd < 0) printf("## ");
            else if (srv->poll->fds[i].fd < 10) printf("0%d ", srv->poll->fds[i].fd);
            else printf("%d ", srv->poll->fds[i].fd);
        }
        putchar('\n');
        fflush(stdout);
        //##############################

        if (has_event(srv, &run) && run) {
            if (new_client_event(srv)) {
                add_new_client(srv);
            }
            for (int32_t i = 1; i < srv->poll->fds_in_use && run; i++) {
                if (existing_client_event(i, srv)) {
                    handle_client(srv, i);
                }
            }
        }

        if (run) {
            timeout_clients(srv);
            compress_fds(srv);
        }
    }
}

void destroy_server(server* srv) {
    destroy_config(srv);

    destroy_socket(srv);

    destroy_client_pool(srv);

    destroy_poll(srv);

    free(srv->buffer);
}
