#ifndef kRBTqiq4rm_SIMPLE_HTTP_CLIENT_HANDLER_H
#define kRBTqiq4rm_SIMPLE_HTTP_CLIENT_HANDLER_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <glib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_client.h"
#include "types/simple_http_type_server.h"
#include "simple_http_util.h"
#include "simple_http_server_io.h"
#include "simple_http_status_codes.h"

void init_client_pool(Server* srv);
void _free_client(gpointer mem);
void add_new_client(Server* srv);

Client* _construct_new_client(Server* srv);
int32_t _accept_connection(Server* srv, Client* client);
void _add_client_to_pool(Server* srv, Client* client, int32_t fd);

void timeout_clients(Server* srv);
void remove_client_from_pool(Server* srv, int32_t index, int32_t fd);

void destroy_client_pool(Server* srv);

#endif
