#ifndef yxPf8IZHjp_SIMPLE_HTTP_APP
#define yxPf8IZHjp_SIMPLE_HTTP_APP

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"

#include "simple_http_server_config.h"
#include "simple_http_server_socket.h"
#include "simple_http_server_poll.h"
#include "simple_http_client_handler.h"
#include "simple_http_server_response.h"

void config_server(server* srv, char* path, int32_t argc, char** argv);
void start_server(server* srv);
void destroy_server(server* srv);

// TODO: ADD_PATH
// TODO: OVERRIDE_DEFAULT_RESPONSE

#endif
