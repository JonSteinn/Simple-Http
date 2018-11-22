#ifndef ET5ztrYeg9_SIMPLE_HTTP_TYPE_CLIENT_H
#define ET5ztrYeg9_SIMPLE_HTTP_TYPE_CLIENT_H

#include <time.h>
#include <netinet/in.h>
#include <glib.h>

typedef struct {
    time_t              last_active;
    struct sockaddr_in  address;
    GString*            raw_request;
} client;

#endif
