#ifndef Cusledit33_SIMPLE_HTTP_TYPE_RESPONSE_H
#define Cusledit33_SIMPLE_HTTP_TYPE_RESPONSE_H

#include <stdint.h>
#include <glib.h>

/**
 * Data structure for response, that is parsed
 * and sent to a client.
 * 
 *  status_code
 *      The status code of the response.
 *  headers
 *      The headers of the response. The following
 *      headers are set automatically by the framework
 *          - Server
 *          - Content-Length
 *          - Date
 *      Also, "Set-Cookie" header will be ignored and
 *      must be set via cookies.
 *  cookies
 *      A collection of values for the "Set-Cookie"
 *      header field
 *  body
 *      The body of the response
 */
typedef struct {
    int32_t         status_code;
    GHashTable*     headers;
    GHashTable*     cookies;
    GString*        body;
} Response;

#endif
