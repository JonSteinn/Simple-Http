#ifndef rsip4NRX14_SIMPLE_HTTP_SERVER_TYPE_H
#define rsip4NRX14_SIMPLE_HTTP_SERVER_TYPE_H

#include <stdint.h>
#include <netinet/in.h>
#include <glib.h>

typedef struct {
    char*                   buffer;
    struct sockaddr_in      address;
    int32_t                 fd;
    GHashTable*             client_pool;
} server;

#endif
