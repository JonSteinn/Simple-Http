/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * Handles any request related logic, e.g. parsing a raw     *
 * request into a request object. It also handles creating,  *
 * restarting and destroying the single request object the   *
 * server holds.                                             *
 *************************************************************/

#ifndef Uva6nmTAHh_SIMPLE_HTTP_CLIENT_REQUEST_H
#define Uva6nmTAHh_SIMPLE_HTTP_CLIENT_REQUEST_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <glib.h>

#include "types/simple_http_type_method.h"
#include "types/simple_http_type_version.h"
#include "types/simple_http_type_request.h"
#include "types/simple_http_type_server.h"

#include "simple_http_util.h"

void init_request(Server* server);
void restart_request(Server* server);
void destroy_request(Server* server);
bool parse_request(Server* server, GString* raw_request);
bool keep_alive(Server* server);

#endif
