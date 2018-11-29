#ifndef UnNdvSL5YU_SIMPLE_HTTP_SERVER_IO_H
#define UnNdvSL5YU_SIMPLE_HTTP_SERVER_IO_H

#include <stdio.h>
#include <stdint.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"
#include "simple_http_server_response.h"

int32_t send_g_string(server* srv, int32_t fd, GString* respsone);
int32_t send_default(server* srv, int32_t fd, int32_t status_code);

#endif
