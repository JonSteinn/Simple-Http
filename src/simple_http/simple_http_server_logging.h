/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * Handle writing and documenting requests to a file.        *
 *************************************************************/

#ifndef H6nJaFCP3h_SIMPLE_HTTP_SERVER_LOGGING_H
#define H6nJaFCP3h_SIMPLE_HTTP_SERVER_LOGGING_H 

#include <stdbool.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "types/simple_http_type_client.h"
#include "types/simple_http_type_server.h"

void init_logging(Server* server);
void destroy_loggging(Server* server);
void log_request(Server* server, Client* client, bool found, bool head);

#endif
