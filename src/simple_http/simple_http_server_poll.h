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

void init_poll(server* srv);
bool has_event(server* srv, bool* run);
bool new_client_event(server* srv);
bool existing_client_event(int32_t index, server* srv);
void compress_fds(server* srv);
void _compress_fds_from_right_end(server* srv);
void _compress_fds_from_left_end(server* srv);
void _remove_excessive_fds(server* srv);
void destroy_poll(server* srv);

#endif
