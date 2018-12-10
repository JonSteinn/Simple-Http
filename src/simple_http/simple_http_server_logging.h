#ifndef H6nJaFCP3h_SIMPLE_HTTP_SERVER_LOGGING_H
#define H6nJaFCP3h_SIMPLE_HTTP_SERVER_LOGGING_H 

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "types/simple_http_type_server.h"

void init_logging(Server* server);
void destroy_loggging(Server* server);

#endif
