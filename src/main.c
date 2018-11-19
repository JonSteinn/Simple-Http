#include <stdint.h>
#include "simple_http/simple_http_app.h"

int32_t main(int32_t argc, char** argv) {
    server my_server;
    config_server(&my_server, "./src/config.txt", argc, argv);
    start_server(&my_server);
    destroy_server(&my_server);
    return 0;
}