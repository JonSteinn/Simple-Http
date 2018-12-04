#ifndef UnNdvSL5YU_SIMPLE_HTTP_SERVER_IO_H
#define UnNdvSL5YU_SIMPLE_HTTP_SERVER_IO_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "types/simple_http_type_client.h"
#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"
#include "simple_http_server_response.h"

void init_io(Server* server);
void destroy_io(Server* server);

int32_t send_g_string(Server* server, int32_t fd, GString* respsone);
int32_t send_default(Server* server, int32_t fd, int32_t status_code);

bool recv_from_client_successs(Server* server, int32_t fd, Client* client, bool* complete);

#endif
