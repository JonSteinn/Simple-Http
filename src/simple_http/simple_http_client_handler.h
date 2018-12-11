/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * Handlees anything related to the client pool.             *
 * Initializing and destroying the client pool, adding new   *
 * clients, TCP handshake, timeout clients removing clients. *
 *************************************************************/

#ifndef kRBTqiq4rm_SIMPLE_HTTP_CLIENT_HANDLER_H
#define kRBTqiq4rm_SIMPLE_HTTP_CLIENT_HANDLER_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <glib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "types/simple_http_type_method.h"
#include "types/simple_http_type_config.h"
#include "types/simple_http_type_client.h"
#include "types/simple_http_type_server.h"

#include "simple_http_util.h"
#include "simple_http_server_io.h"
#include "simple_http_status_codes.h"
#include "simple_http_server_response.h"
#include "simple_http_client_request.h"
#include "simple_http_server_routes.h"
#include "simple_http_server_static_files.h"
#include "simple_http_server_prints.h"
#include "simple_http_server_logging.h"

void init_client_pool(Server* srv);
void add_new_client(Server* srv);
void timeout_clients(Server* srv);
void remove_client_from_pool(Server* srv, int32_t index, int32_t fd);
void proccess_client_event(Server* server, int32_t i);
void destroy_client_pool(Server* srv);

#endif
