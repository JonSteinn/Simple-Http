#ifndef Uva6nmTAHh_SIMPLE_HTTP_CLIENT_REQUEST_H
#define Uva6nmTAHh_SIMPLE_HTTP_CLIENT_REQUEST_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <glib.h>

#include "types/simple_http_type_request.h"
#include "types/simple_http_type_server.h"
#include "simple_http_util.h"

void init_request(Server* server);
void restart_request(Server* server);
void destroy_request(Server* server);

bool parse_request(Server* server, GString* raw_request);

bool _set_http_version(Server* server, char* version);
void _parse_queries(Server* server, char* query_string);
bool _parse_url(Server* server, char* url);
bool _set_http_method(Server* server, char* method);
bool _parse_start_line(Server* server, char* line);
void _parse_headers(Server* server, char** lines);
void _parse_cookie(Server* server, char* cookie);

#endif
