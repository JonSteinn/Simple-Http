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

void init_client_pool(server* srv);
void _free_client(gpointer mem);
void add_new_client(server* srv);

client* _construct_new_client();
int32_t _accept_connection(server* srv, client* new_client);
void _add_client_to_pool(server* srv, client* new_client, int32_t fd);

void handle_client(server* srv, int32_t index);

void timeout_clients(server* srv);
void _remove_client_from_pool(server* srv, int32_t i, int32_t fd);

void destroy_client_pool(server* srv);

#endif
