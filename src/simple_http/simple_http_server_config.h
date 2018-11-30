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

void init_config(Server* server, char* path, int32_t argc, char** argv);
void _config_set_default(Config* cfg);
void _config_set_from_file(GHashTable* settings, char* path);
void _config_parse_line(GHashTable* settings, char* line, int32_t chars_read);
void _config_split_line(GHashTable* settings, char* line, int32_t chars_read, int32_t run, int32_t split);
void _config_set_from_args(GHashTable* settings, int32_t argc, char** argv);
void _config_set(Config* cfg, GHashTable* settings);
void _config_set_debug(Config* cfg, char* value);
void _config_set_port(Config* cfg, char* value);
void _config_set_max_queued(Config* cfg, char* value);
void _config_set_max_clients(Config* cfg, char* value);
void _config_set_buffer_size(Config* cfg, char* value);
void _config_set_ip(Config* cfg, char* value);
void _config_set_poll_timeout(Config* cfg, char* value);
void _config_set_inactive_timeout(Config* cfg, char* value);
// <-------- SETTERS
void _config_display(Config* cfg);
void destroy_config(Server* server);

#endif
