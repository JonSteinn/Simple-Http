#ifndef RNp03QjYHQ_SIMPLE_HTTP_TYPE_VERSION_H
#define RNp03QjYHQ_SIMPLE_HTTP_TYPE_VERSION_H

/**
 * Enum for http version.
 * 
 * HTTP/1.0     =0
 * HTTP/1.1     =1
 * HTTP/2.0     =2
 */
typedef enum {
    VERSION_10,
    VERSION_11,
    VERSION_20
} Version;

#endif