#ifndef yxPf8IZHjp_SIMPLE_HTTP_APP
#define yxPf8IZHjp_SIMPLE_HTTP_APP

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include "./types/simple_http_type_server.h"

void config_server(server* srv, char* path, int32_t argc, char** argv);
void start_server(server* srv);
void destroy_server(server* srv);

#endif