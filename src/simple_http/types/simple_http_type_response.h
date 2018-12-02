#ifndef Cusledit33_SIMPLE_HTTP_TYPE_RESPONSE_H
#define Cusledit33_SIMPLE_HTTP_TYPE_RESPONSE_H

#include <stdint.h>
#include <glib.h>

/**
 * 
 */
typedef struct {
    int32_t         status_code;
    GHashTable*     headers;
    GHashTable*     cookies;
    GString*        body;
} Response;

#endif
