/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * Sets various configurations for the server, either by     *
 * default or set by the framework user with a config file   *
 * or command line arguments.                                *
 *************************************************************/

#ifndef pOPm9Npa50_SIMPLE_HTTP_SERVER_CONFIG_H
#define pOPm9Npa50_SIMPLE_HTTP_SERVER_CONFIG_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <glib.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"

#include "simple_http_server_prints.h"

void init_config(Server* server, char* path, int32_t argc, char** argv);
void destroy_config(Server* server);

#endif
