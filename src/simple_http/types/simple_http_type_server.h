#ifndef rsip4NRX14_SIMPLE_HTTP_SERVER_TYPE_H
#define rsip4NRX14_SIMPLE_HTTP_SERVER_TYPE_H

#include <stdint.h>
#include <netinet/in.h>
#include <glib.h>

#include "simple_http_type_config.h"
#include "simple_http_type_polling.h"
#include "simple_http_type_response.h"
#include "simple_http_type_request.h"

typedef struct {
    Config*                 cfg;
    Polling*                poll;
    char*                   buffer;
    struct sockaddr_in      address;
    int32_t                 fd;
    GHashTable*             client_pool;
    GHashTable*             default_response;
    Response*               response;
    Request*                request;
} Server;

#endif
