/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * This holds all methods the framework user is expected to  *
 * use. It serves as a kind of interface.                    *
 *************************************************************/

#ifndef yxPf8IZHjp_SIMPLE_HTTP_APP
#define yxPf8IZHjp_SIMPLE_HTTP_APP

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>

#include "types/simple_http_type_method.h"
#include "types/simple_http_type_route.h"
#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"

#include "simple_http_util.h"
#include "simple_http_server_config.h"
#include "simple_http_server_socket.h"
#include "simple_http_server_poll.h"
#include "simple_http_client_handler.h"
#include "simple_http_server_response.h"
#include "simple_http_client_request.h"
#include "simple_http_server_routes.h"
#include "simple_http_server_static_files.h"

void config_server(Server* server, char* path, int32_t argc, char** argv);
void add_route(Server* server, Method method, const char* path, route_function function);
void start_server(Server* server);
void destroy_server(Server* server);

#endif
