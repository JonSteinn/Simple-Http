/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * This file contains socket setup for the server.           *
 *************************************************************/

#ifndef A90g4prKzV_SIMPLE_HTTP_SERVER_SOCKET_H
#define A90g4prKzV_SIMPLE_HTTP_SERVER_SOCKET_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"

#include "simple_http_server_prints.h"

void init_socket(Server* server, bool* run);
void _initialize_address(Server* server);
void _setup_socket(Server* server);
void _bind_socket(Server* server);
void _listen_on_socket(Server* server);
void destroy_socket(Server* server);

#endif
