/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * This file contains any logic to help serve static files   *
 * to the client.                                            *
 *************************************************************/

#ifndef AMe618mW6x_SIMPLE_HTTP_SERVER_STATIC_FILES_H
#define AMe618mW6x_SIMPLE_HTTP_SERVER_STATIC_FILES_H

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_response.h"
#include "types/simple_http_type_method.h"
#include "types/simple_http_type_server.h"
#include "types/simple_http_type_static_files.h"

#include "simple_http_server_response.h"
#include "simple_http_status_codes.h"

void init_static_files(Server* server);
void destroy_static_files(Server* server);
bool read_file_into_response(Server* server);

#endif

