#ifndef Fxjyb5PNWQ_SIMPLE_HTTP_SERVER_ROUTES_H
#define Fxjyb5PNWQ_SIMPLE_HTTP_SERVER_ROUTES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <glib.h>

#include "types/simple_http_type_method.h"
#include "types/simple_http_type_route.h"
#include "types/simple_http_type_server.h"
#include "simple_http_util.h"

void init_routes(Server* server);
void _free_routes(gpointer mem);
void destroy_routes(Server* server);

void add_new_route(Server* server, Method method, route_function function, const char* path);
bool find_route_and_call_its_method(Server* server);

#endif
