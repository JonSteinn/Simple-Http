#include "simple_http_server_routes.h"

/**
 * Initialize an array of route dictionaries. Each entry in the
 * array corresponds to a single http method (GET/HEAD/etc). Each
 * dictionary maps strings to a sub-dictionary and a function.
 */
void init_routes(Server* server) {
    server->routes = (GHashTable**)malloc(sizeof(GHashTable*) * 9);
    for (int32_t i = 0; i < 9; i++) {
        server->routes[i] = g_hash_table_new_full(g_str_hash, g_str_equal, free, _free_routes);
    }
}

/**
 * Value de-allocator for route dictionaries.
 */
void _free_routes(gpointer mem) {
    route_part* part = (route_part*)mem;
    if (part->next != NULL) g_hash_table_destroy(part->next);
    free(part); 
}

/**
 * De-allocate the array of route dictionaries.
 */
void destroy_routes(Server* server) {
    for (int32_t i = 0; i < 9; i++) {
        g_hash_table_destroy(server->routes[i]);
    }
    free(server->routes);
}

/**
 * Add an endpoint for a given method and a callback function.
 */
bool add_new_route(Server* server, Method method, route_function callback, const char* path) {
    GHashTable* dictionary = server->routes[method];
    char** parts = g_strsplit(path, "/", -1);
    bool is_valid = true;
    
    if (parts[0] == NULL) {
        _home_add_new_route(dictionary, callback);
    } else {
        is_valid = _sub_add_new_route(parts, dictionary, callback);
    }
    
    g_strfreev(parts);
    return is_valid;
}

/**
 * Helper for adding a callback to the home path ("/").
 */
void _home_add_new_route(GHashTable* dictionary, route_function function) {
    route_part* part = (route_part*)g_hash_table_lookup(dictionary, "");
    if (part == NULL) {
        g_hash_table_insert(dictionary, g_strdup(""), _alloc_new_route_part(false, function));
    } else {
        part->function = function;
    }
}

/**
 * Allocate an set a new route_part. If init_next is true, we initialize
 * the sub dictionary as well. If the function is not NULL, we store it
 * in the route_part.
 */
route_part* _alloc_new_route_part(bool init_next, route_function function) {
    route_part* part = (route_part*)malloc(sizeof(route_part));
    part->function = function == NULL ? NULL : function;
    part->next = init_next ? g_hash_table_new_full(g_str_hash, g_str_equal, free, _free_routes) : NULL;
    return part;
}

/**
 * Helper for adding a callback to any other path than the home path ("/"). If the 
 * path is valid, we return true, otherwise false.
 */
bool _sub_add_new_route(char** parts, GHashTable* dictionary, route_function function) {
    int32_t i = 0;
    for (i = 0; parts[i] != NULL && parts[i + 1] != NULL; i++) {
        // Missing or includes bracket without being {arg}.
        if (!parts[i][0] || (contains_curly_brackets(parts[i]) && g_ascii_strcasecmp(parts[i], "{arg}"))) {
            return false;
        }

        // Dive
        route_part* part = (route_part*)g_hash_table_lookup(dictionary, parts[i]);
        if (part == NULL) {
            part = _alloc_new_route_part(true, NULL);
            g_hash_table_insert(dictionary, g_strdup(parts[i]), part);
        } else if (part->next == NULL) {
            part->next = g_hash_table_new_full(g_str_hash, g_str_equal, free, _free_routes);
        }
        dictionary = part->next;
    }

    // If invalid
    if (!parts[i][0] || (contains_curly_brackets(parts[i]) && g_ascii_strcasecmp(parts[i], "{arg}"))) {
        return false;
    }
    
    // Store pointer to function
    route_part* part = (route_part*)g_hash_table_lookup(dictionary, parts[i]);
    if (part == NULL) {
        g_hash_table_insert(dictionary, g_strdup(parts[i]), _alloc_new_route_part(false, function));
    } else {
        part->function = function;
    }

    return true;
}






















bool find_route_and_call_its_method(Server* server) {
    GHashTable* dictionary = server->routes[server->request->method];
    char** parts = g_strsplit(server->request->path->str, "/", -1);
    bool found = true;

    // HOME
    if (parts[0] == NULL) {
        route_part* part = (route_part*)g_hash_table_lookup(dictionary, "");
        if (part == NULL) {
            found = false;
        } else {
            part->function(server->request, server->response, 0, NULL);
        }
        g_strfreev(parts);
        return found;
    }

    int32_t argument_counter = 0;
    int32_t array_size = 10;
    char** arguments = (char**)malloc(sizeof(char*) * array_size);

    int32_t i = 0;
    for (i = 0; parts[i] != NULL && parts[i + 1] != NULL; i++) {       
         
        route_part* part = (route_part*)g_hash_table_lookup(dictionary, parts[i]);
        if (part == NULL) {
            if (parts[i][0] && (part = (route_part*)g_hash_table_lookup(dictionary, "{arg}")) != NULL) {
                if (argument_counter == array_size) {
                    array_size <<= 1;
                    arguments = realloc(arguments, sizeof(char*) * array_size);
                }
                arguments[argument_counter++] = g_strdup(parts[i]);
            }
        }

        if (part == NULL || part->next == NULL) {
            found = false;
            break;
        } else {
            dictionary = part->next;
        }
    }

    if (found) {
        route_part* part = (route_part*)g_hash_table_lookup(dictionary, parts[i]);
        if (part == NULL) {
            if (parts[i][0] && (part = (route_part*)g_hash_table_lookup(dictionary, "{arg}")) != NULL) {
                if (argument_counter == array_size) {
                    array_size <<= 1;
                    arguments = realloc(arguments, sizeof(char*) * array_size);
                }
                arguments[argument_counter++] = g_strdup(parts[i]);
            }
        }
        if (part == NULL || part->function == NULL) {
            found = false;
        } else {
            part->function(server->request, server->response, argument_counter, arguments);
        }
    }
    for (int32_t i = 0; i < argument_counter; i++) {
        free(arguments[i]);
    }
    free(arguments);
    g_strfreev(parts);

    return found;
}


/**
 * Validate routes set by framework user.
 */
bool validate_route(const char* path) {
    size_t len = strlen(path);
    for (size_t i = 0; i < len; i++) {
        if ((path[i] >= 'a' && path[i] <= 'z') ||
            (path[i] >= 'A' && path[i] <= 'Z') ||
            (path[i] >= '0' && path[i] <= '9')) {
            
            continue;
        }

        char* legal = "/{}$-_.+!*'()";
        size_t legal_len = strlen(legal);
        bool valid = false;

        for (size_t j = 0; j < legal_len; j++) {
            if (path[i] == legal[j]) {
                valid = true;
                break;
            }
        }
        
        if (!valid) {
            return false;
        }
    }
    return true;
}