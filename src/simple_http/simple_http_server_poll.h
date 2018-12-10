/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * Handles multiplexing via poll. Provides a simple          *
 * interface on when some event has occured. It also handles *
 * compressing of the poll data structure                    *
 *************************************************************/

#ifndef pi1aNn1Ex6_SIMPLE_HTTP_SERVER_POLL_H
#define pi1aNn1Ex6_SIMPLE_HTTP_SERVER_POLL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "types/simple_http_type_polling.h"
#include "types/simple_http_type_config.h"
#include "types/simple_http_type_server.h"

#include "simple_http_server_prints.h"

void init_poll(Server* server);
bool has_event(Server* server, bool* run);
bool new_client_event(Server* server);
bool existing_client_event(int32_t index, Server* server);
void compress_fds(Server* server);
void _compress_fds_from_right_end(Server* server);
void _compress_fds_from_left_end(Server* server);
void _remove_excessive_fds(Server* server);
void destroy_poll(Server* server);

#endif
