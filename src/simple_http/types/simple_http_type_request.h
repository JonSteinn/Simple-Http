#ifndef B4Oqf3sHtY_SIMPLE_HTTP_TYPE_REQUEST_H
#define B4Oqf3sHtY_SIMPLE_HTTP_TYPE_REQUEST_H

#include <glib.h>

#include "simple_http_type_method.h"
#include "simple_http_type_version.h"

/**
 * Data structure for a parsed request from a client.
 * 
 *  version
 *      Which http version (1.0, 1.1, 2.1)
 *  method
 *      Which http method (GET, HEAD, POST, ...)
 *  queries
 *      The queries split into key and value in a dictionary
 *  headers
 *      The headers key and value in a dictionary. Any "Cookie" header
 *      will be places into cookies but not here.
 *  cookies
 *      Cookies in a dictionary. Those without value are mapped to the
 *      empty string.
 *  path
 *      The path of the url (<protocol>://<domain><path>?<query>#<fragment>)
 *  fragment
 *      The fragment of the url (<protocol>://<domain><path>?<query>#<fragment>)
 *  body
 *      The body, if any, of the request
 */
typedef struct {
    Version         version;
    Method          method;
    GHashTable*     queries;
    GHashTable*     headers;
    GHashTable*     cookies;
    GString*        path;
    GString*        fragment;
    GString*        body;
} Request;

#endif
