#include "simple_http/simple_http_app.h"

int32_t main(int32_t argc, char** argv) {
    Server server;
    config_server(&server, "./src/config.txt", argc, argv);

    start_server(&server);
    destroy_server(&server);
    return 0;
}
