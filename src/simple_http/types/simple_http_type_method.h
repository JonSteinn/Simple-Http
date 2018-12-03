#ifndef aEwe2BLiT2_SIMPLE_HTTP_TYPE_METHOD_H
#define aEwe2BLiT2_SIMPLE_HTTP_TYPE_METHOD_H

/**
 * Enum for http methods.
 * 
 * GET          =0
 * HEAD         =1
 * POST         =2
 * PUT          =3
 * DELETE       =4
 * CONNECTION   =5
 * OPTIONS      =6
 * TRACE        =7
 * PATCH        =8
 */
typedef enum {
    METHOD_GET,
    METHOD_HEAD,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_CONNECTION,
    METHOD_OPTIONS,
    METHOD_TRACE,
    METHOD_PATCH
} Method;

#endif
