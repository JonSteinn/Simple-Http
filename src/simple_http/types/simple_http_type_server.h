#ifndef rsip4NRX14_SIMPLE_HTTP_SERVER_TYPE_H
#define rsip4NRX14_SIMPLE_HTTP_SERVER_TYPE_H

#include <stdint.h>
#include <netinet/in.h>
#include <glib.h>

#include "simple_http_type_config.h"
#include "simple_http_type_polling.h"
#include "simple_http_type_response.h"
#include "simple_http_type_request.h"

/**
 * Data structure for the server it self. It contains
 * all variables used in this framework beside the 
 * global variable for keeping the server loop running
 * which is global so it can be set to false by a 
 * signal handler.
 * 
 *  cfg
 *      A collection of configurations
 *  poll
 *      The variables needed for polling
 *  buffer
 *      The buffer used to read from sockets
 *  address
 *      The address family, port and ip for the server
 *  fd
 *      The file descriptor for the server (also stored
 *      in server->poll->fds[0].fd, in the polling data
 *      structure but kept here as well for convenience)
 *  client_pool
 *      A hash map of fd to their client data structure
 *  default_response
 *      A hash map of status codes to their default
 *      response as a GString*.
 *  response
 *      A single response object that is shared between
 *      all clients and cleared as soon as it has been
 *      sent. The framework user can edit this response
 *      as he sees fit.
 *  request
 *      A single request object that is shared between
 *      all clients and cleared as soon as the client
 *      has been sent a response. When there is nothing
 *      more to read on a socket we parse the raw request
 *      to this request object, which the framework user
 *      has access to but should not modify.
 *  routes
 *      An array of dictionaries, one for each supported
 *      http method type. It contains all defined routes
 *      that the server supports.
 */
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
    GHashTable**            routes;
} Server;

#endif
