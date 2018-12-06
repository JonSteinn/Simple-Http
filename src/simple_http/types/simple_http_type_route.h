#ifndef wlwKdUHzwZ_SIMPLE_HTTP_TYPE_ROUTE_H
#define wlwKdUHzwZ_SIMPLE_HTTP_TYPE_ROUTE_H

#include <glib.h>
#include <stdint.h>

#include "simple_http_type_response.h"
#include "simple_http_type_request.h"

// The type of function the framework user must define 
typedef void (*route_function)(Request* req, Response* res, int32_t argc, char** argv);

/**
 * Data structure for the dictionary that holds routes.
 * It nests each part (split with delimiter = '/' ignoring
 * the leading and trailing) of the route, e.g. 
 *      "/home/dostuff/test"
 * would be contained in the following object:
 *      {"home": {"dostuff": {"test": NULL, func: ...}, func: ...}, func: ...}
 * 
 *  function
 *      The function defined by the framework user that sets
 *      the response given a request, response and possibly
 *      path parameters.
 *  next
 *      Nested dictionary that maps string into route_part.
 */
typedef struct {
    route_function  function;
    GHashTable*     next;
} route_part;

#endif
