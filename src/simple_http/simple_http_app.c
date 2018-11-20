#include "simple_http_app.h"

static bool run = true;
static void _int_handler(int32_t sig) {
    if (sig == SIGINT) run = false;
    putchar('\n');
}


void config_server(server* srv, char* path, int32_t argc, char** argv) {
    signal(SIGINT, _int_handler);

    init_config(srv, path, argc - 1, argv + 1);
    init_socket(srv);
}

void start_server(server* srv) {
    printf("(Temp warning repressor %d)\n", srv->fd);
    printf("Starting server!\n");
}

void destroy_server(server* srv) {
    destroy_config(srv);
    destroy_socket(srv);
}
