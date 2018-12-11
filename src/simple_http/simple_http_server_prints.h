/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * Handles anything that is put to either the standard out   *
 * or the standard err.                                      *
 *************************************************************/

#ifndef JwOUYdG3Ag_SIMPLE_HTTP_SERVER_PRINTS_H
#define JwOUYdG3Ag_SIMPLE_HTTP_SERVER_PRINTS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <glib.h>

#include "types/simple_http_type_client.h"
#include "types/simple_http_type_config.h"
#include "types/simple_http_type_method.h"
#include "types/simple_http_type_polling.h"
#include "types/simple_http_type_request.h"
#include "types/simple_http_type_response.h"
#include "types/simple_http_type_route.h"
#include "types/simple_http_type_server.h"
#include "types/simple_http_type_static_files.h"
#include "types/simple_http_type_version.h"

// Always prints
void sh_print_shutdown();
void sh_print_start_message(Server* server, bool run);

// Debug only
void sh_print_config(Server* server);
void sh_print_socket(Server* server);
void sh_print_config_done(Server* server, bool run);
void sh_print_adding_route(Server* server, const char* path, bool valid);
void sh_print_accept_failed(Server* server);
void sh_print_no_room(Server* server);
void sh_print_new_client(Server* server, Client* client);
void sh_print_timeout(Server* server, int32_t fd);
void sh_print_remove_client(Server* server, int32_t fd);
void sh_print_failed_to_recv(Server* server, Client* client);
void sh_print_parse_fail(Server* server, Client* client);
void sh_print_request_response(Server* server, Client* client, bool found, bool head);
void sh_print_fd_array(Server* server, bool before_compress);

// Error prints
void sh_print_signal_error();
void sh_print_socket_error();
void sh_print_bind_error();
void sh_print_listen_error();
void sh_print_poll_error(bool run);
void sh_print_send_error(Server* server);

#endif