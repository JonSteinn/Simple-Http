/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * Handles anything response related that is not handled     *
 * directly by the framework user. It initializes, restarts  *
 * and destroys the single response object that the server   *
 * holds. It also initializes and destroys default responses *
 *************************************************************/

#ifndef Y8LbUPvRqy_SIMPLE_HTTP_SERVER_RESPONSE_H
#define Y8LbUPvRqy_SIMPLE_HTTP_SERVER_RESPONSE_H

#include <stdbool.h>
#include <stdlib.h>
#include <glib.h>

#include "types/simple_http_type_response.h"
#include "types/simple_http_type_server.h"
#include "simple_http_util.h"

void init_resposne(Server* server);
void restart_response(Server* server);
void destroy_response(Server* server);

void init_default_responses(Server* server);
void _free_default_response(gpointer mem);
void destroy_default_responses(Server* server);

#endif