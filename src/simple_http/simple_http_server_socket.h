#ifndef A90g4prKzV_SIMPLE_HTTP_SERVER_SOCKET_H
#define A90g4prKzV_SIMPLE_HTTP_SERVER_SOCKET_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"

void init_socket(Server* server);
void _initialize_address(Server* server);
void _setup_socket(Server* server);
void _bind_socket(Server* server);
void _listen_on_socket(Server* server);
void _socket_display(Server* server);
void destroy_socket(Server* server);

#endif
