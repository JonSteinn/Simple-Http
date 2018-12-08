#ifndef AMe618mW6x_SIMPLE_HTTP_SERVER_STATIC_FILES_H
#define AMe618mW6x_SIMPLE_HTTP_SERVER_STATIC_FILES_H

//#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

#include "types/simple_http_type_response.h"
#include "types/simple_http_type_method.h"
#include "types/simple_http_type_server.h"
#include "types/simple_http_type_static_files.h"
#include "simple_http_server_response.h"

void init_static_files(Server* server);
void _add_supported_media_types(Server* server);
void _no_free(__attribute__((unused)) gpointer mem);
void _find_static_files_recursive(DIR* dir, const char* prefix, StaticFiles* static_files);
bool _valid_extension(GString* file, GHashTable* supported);
void destroy_static_files(Server* server);

bool read_file_into_response(Server* server);
FILE* _open_file(const char* file_path);

#endif

