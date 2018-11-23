#ifndef Cusledit33_SIMPLE_HTTP_TYPE_RESPONSE_H
#define Cusledit33_SIMPLE_HTTP_TYPE_RESPONSE_H

#include <stdint.h>
#include <glib.h>

typedef struct {
    int32_t status_code;
    GHashTable* headers;
    GHashTable* cookies;
    GString* body;
} response;

/*GString* create_response(response* res) {
    GString* r = g_string_new_len("HTTP/1.1 ", 1000);

}*/


#endif
