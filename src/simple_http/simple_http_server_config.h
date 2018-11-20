#ifndef pOPm9Npa50_SIMPLE_HTTP_SERVER_CONFIG_H
#define pOPm9Npa50_SIMPLE_HTTP_SERVER_CONFIG_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <glib.h>

#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"

void init_config(server* srv, char* path, int32_t argc, char** argv);
void _config_set_default(config* cfg);
void _config_set_from_file(GHashTable* settings, char* path);
void _config_parse_line(GHashTable* settings, char* line, int32_t chars_read);
void _config_split_line(GHashTable* settings, char* line, int32_t chars_read, int32_t run, int32_t split);
void _config_set_from_args(GHashTable* settings, int32_t argc, char** argv);
void _config_set(config* cfg, GHashTable* settings);
void _config_set_debug(config* cfg, char* value);
void _config_set_port(config* cfg, char* value);
void _config_set_max_queued(config* cfg, char* value);
void _config_set_max_clients(config* cfg, char* value);
void _config_set_buffer_size(config* cfg, char* value);
void _config_set_ip(config* cfg, char* value);
void _config_set_poll_timeout(config* cfg, char* value);
// <-------- SETTERS
void _config_display(config* cfg);
void destroy_config(server* srv);

#endif
