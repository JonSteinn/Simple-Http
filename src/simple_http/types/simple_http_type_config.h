#ifndef U2O6oUWIRu_SIMPLE_HTTP_TYPE_CONFIG_H
#define U2O6oUWIRu_SIMPLE_HTTP_TYPE_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

/**
 * A collection of various configuration variables.
 *  debug
 *      Display additional info in stdout
 *  port
 *      Port for server (<protocol>://<ip>:<port>/<url>)
 *  max_queued
 *      The max queue size of pending connections 
 *      for our socket may grow
 *  max_clients
 *      How many clients can be alive at once
 *  buffer_size
 *      The size + 1 of the chunk read at a time
 *      from clients
 *  poll_timeout
 *      How long before poll returns without events.
 *  ip
 *      IP for the server (<protocol>://<ip>:<port>/<url>)
 *  inactive_timeout
 *      How long clients can go without making a request
 *      before they are removed and disconnected
 */
typedef struct {
    bool        debug;
    uint16_t    port;
    int32_t     max_queued;
    int32_t     max_clients;
    int32_t     buffer_size;
    int32_t     poll_timeout;
    uint32_t    ip;
    int32_t     inactive_timeout;
} Config;

#endif
