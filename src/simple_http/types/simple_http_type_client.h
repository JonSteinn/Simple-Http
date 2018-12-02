#ifndef ET5ztrYeg9_SIMPLE_HTTP_TYPE_CLIENT_H
#define ET5ztrYeg9_SIMPLE_HTTP_TYPE_CLIENT_H

#include <time.h>
#include <netinet/in.h>
#include <glib.h>

/**
 * Data structure for client.
 *  last_active
 *      The last time the client did 
 *  address
 *      IP and port for client
 * raw_request
 *      A string to append received buffer to
 */
typedef struct {
    time_t              last_active;
    struct sockaddr_in  address;
    GString*            raw_request;
} Client;

#endif
