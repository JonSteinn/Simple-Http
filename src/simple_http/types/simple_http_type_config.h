#ifndef U2O6oUWIRu_SIMPLE_HTTP_TYPE_CONFIG_H
#define U2O6oUWIRu_SIMPLE_HTTP_TYPE_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool        debug;
    uint16_t    port;
    int32_t     max_queued;
    int32_t     max_clients;
    int32_t     buffer_size;
    int32_t     poll_timeout;
    uint32_t    ip;
    int32_t     inactive_timeout;
} config;

#endif
