#ifndef A90g4prKzV_SIMPLE_HTTP_SERVER_SOCKET_H
#define A90g4prKzV_SIMPLE_HTTP_SERVER_SOCKET_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"

void init_socket(server* srv);
void _initialize_address(server* srv);
void _setup_socket(server* srv);
void _bind_socket(server* srv);
void _listen_on_socket(server* srv);
void _socket_display(server* srv);
void destroy_socket(server* srv);

#endif
