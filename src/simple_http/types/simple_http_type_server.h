#ifndef rsip4NRX14_SIMPLE_HTTP_SERVER_TYPE_H
#define rsip4NRX14_SIMPLE_HTTP_SERVER_TYPE_H

#include <stdint.h>
#include <netinet/in.h>
#include <glib.h>

#include "simple_http_type_config.h"
#include "simple_http_type_polling.h"

typedef struct {
    config*                 cfg;
    polling*                poll;
    char*                   buffer;
    struct sockaddr_in      address;
    int32_t                 fd;
    GHashTable*             client_pool;
    GHashTable*             default_response;
} server;

#endif
