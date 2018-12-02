#ifndef d7lpNZRhVW_SIMPLE_HTTP_TYPE_POLLING_H
#define d7lpNZRhVW_SIMPLE_HTTP_TYPE_POLLING_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/poll.h>

/**
 * 
 */
typedef struct {
    struct pollfd*  fds;
    int32_t         fds_in_use;
    bool            compress;
} Polling;

#endif
