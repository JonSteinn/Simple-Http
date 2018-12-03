#ifndef d7lpNZRhVW_SIMPLE_HTTP_TYPE_POLLING_H
#define d7lpNZRhVW_SIMPLE_HTTP_TYPE_POLLING_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/poll.h>

/**
 * Data structure for pollfd array and supporting variable.
 * 
 *  fds
 *      Array of pollfd data type which is required 
 *      to use the poll function
 *  fds_in_use
 *      Tracks how many socket fds are in use, 
 *      including the server's fd
 *  compress
 *      If a client has been removed and thus, a fd 
 *      closed (and set to -1) we set this to true
 *      so the in-use fds can be compressed to occupy
 *      the first places of the array
 */
typedef struct {
    struct pollfd*  fds;
    int32_t         fds_in_use;
    bool            compress;
} Polling;

#endif
