#include "simple_http_server_logging.h"

void init_logging(Server* server) {
    struct stat st = {0};
    if (stat("./log", &st) == -1) {
        mkdir("./log", 0700);
    }

    server->logger = fopen("./log/server-logs.log", "w");
}

void destroy_loggging(Server* server) {
    fclose(server->logger);
}