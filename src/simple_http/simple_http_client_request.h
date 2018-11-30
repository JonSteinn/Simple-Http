#ifndef Uva6nmTAHh_SIMPLE_HTTP_CLIENT_REQUEST_H
#define Uva6nmTAHh_SIMPLE_HTTP_CLIENT_REQUEST_H

#include <stdlib.h>
#include <glib.h>

#include "types/simple_http_type_request.h"
#include "types/simple_http_type_server.h"

void init_request(Server* server);
void restart_request(Server* server);
void destroy_request(Server* server);

#endif
