#ifndef B4Oqf3sHtY_SIMPLE_HTTP_TYPE_REQUEST_H
#define B4Oqf3sHtY_SIMPLE_HTTP_TYPE_REQUEST_H

#include <stdint.h>
#include <glib.h>

#include "simple_http_type_method.h"
#include "simple_http_type_version.h"

typedef struct {
    Version version;
    Method method;
    GHashTable* queries;
    GHashTable* headers;
    GHashTable* cookies;
    GString* path;
    GString* fragment;
    GString* body;
} Request;

#endif
