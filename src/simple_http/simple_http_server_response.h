#ifndef Y8LbUPvRqy_SIMPLE_HTTP_SERVER_RESPONSE_H
#define Y8LbUPvRqy_SIMPLE_HTTP_SERVER_RESPONSE_H

#include <glib.h>

#include "types/simple_http_type_server.h"
#include "simple_http_util.h"

void init_default_responses(server* srv);
void _free_default_response(gpointer mem);
void destroy_default_responses(server* srv);


#endif