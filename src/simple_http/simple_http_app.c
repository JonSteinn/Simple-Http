#include "simple_http_app.h"

// Defines
#define CLIENT_START_INDEX 1

// 'Private' function definitions
void _initialize_all(Server* server, char* path, int32_t argc, char** argv);
bool _invalid_route(Method method, const char* path, route_function function);
bool _invalid_method(Method method);
bool _add_valid_route(Server* server, Method method, const char* path, route_function function);
void _server_loop(Server* server);
void _destroy_all(Server* server);

// Static global variable for keeping loop running.
static bool run = false;

/**
 * Called on ctrl+C event. Sets global run variable to false.
 */
static void _int_handler(int32_t sig) {
    if (sig == SIGINT) {
        run = false;
        sh_print_shutdown();
    }
}

/**
 * Sets up server. This must be called before adding paths, starting server
 * and destroying server. It takes a constructed server (who's memory must
 * be managed by the caller), a path to a config file and the program's args.
 */
void config_server(Server* server, char* path, int32_t argc, char** argv) {
    if (run) {
        return;
    }
    if (!(run = signal(SIGINT, _int_handler) != SIG_ERR)) {
        sh_print_signal_error();
        return;
    }
    _initialize_all(server, path, argc, argv);
    sh_print_config_done(server, run);
}

/**
 * Add an endpoint to the server if method is valid. Any
 * {param} part of the path is passed as arguments to the
 * function, e.g. defining "/home/dostuff/{param}/" would
 * pass "stuff" as argument when "/home/dostuff/stuff" is
 * requested.
 */
void add_route(Server* server, Method method, const char* path, route_function function) {
    bool valid = !_invalid_route(method, path, function) && _add_valid_route(server, method, path, function);
    sh_print_adding_route(server, path, valid);
}

/**
 * Starts server. Config must have been called before this function
 * is called.
 */
void start_server(Server* server) {
    sh_print_start_message(server, run);
    _server_loop(server);
}

/**
 * Releases all resources of the server. Note that it does not free 
 * the server it self. That responsibility falls on the caller.
 */
void destroy_server(Server* server) {
    if (run) {
        return;
    }
    _destroy_all(server);
}

/////////////////////
// Private helpers //
/////////////////////

/**
 * Call init for all parts.
 */
void _initialize_all(Server* server, char* path, int32_t argc, char** argv) {
    init_config(server, path, argc - 1, argv + 1);
    init_socket(server, &run);
    init_poll(server);
    init_client_pool(server);
    init_default_responses(server);
    init_request(server);
    init_resposne(server);
    init_io(server);
    init_routes(server);
    init_static_files(server);
    init_logging(server);
}

/**
 * Check if route parameters are valid.
 */
bool _invalid_route(Method method, const char* path, route_function function) {
    return _invalid_method(method) || path == NULL || function == NULL || !validate_route(path);
}

/**
 * Check if method is okay to define route for.
 */
bool _invalid_method(Method method) {
    return method < 0 || method > 8 || method == METHOD_HEAD;
}

/**
 * Add route given that the first check is succesful. It can fail,
 * if route is invalid in such a way that it isn't caught by the
 * first check. Returns true iff successful.
 */
bool _add_valid_route(Server* server, Method method, const char* path, route_function function) {
    size_t len = string_length_without_trailing_forward_slash(path);
    size_t offset = string_offset_jumping_leading_forward_slash(path, len);
    char* str = g_strndup(path + offset, len - offset);
    bool valid = add_new_route(server, method, function, str);
    free(str);
    return valid;
}

/**
 * The server loop that keeps going while the server is alive.
 */
void _server_loop(Server* server) {
    while(run) {
        if (has_event(server, &run) && run) {
            if (new_client_event(server)) {
                add_new_client(server);
            }
            for (int32_t i = CLIENT_START_INDEX; i < server->poll->fds_in_use && run; i++) {
                if (existing_client_event(i, server)) {
                    proccess_client_event(server, i);
                }
            }
        }
        // Cleanup
        if (run) {
            timeout_clients(server);
            compress_fds(server);
        }
    }
}

/**
 * Call destroy for all parts.
 */
void _destroy_all(Server* server) {
    destroy_config(server);
    destroy_socket(server);    
    destroy_client_pool(server);    
    destroy_poll(server);
    destroy_default_responses(server);
    destroy_response(server);
    destroy_request(server);
    destroy_io(server);
    destroy_routes(server);
    destroy_static_files(server);
    destroy_loggging(server);
}

// remove defines
#undef CLIENT_START_INDEX