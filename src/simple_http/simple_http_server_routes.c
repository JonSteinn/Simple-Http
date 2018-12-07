#include "simple_http_server_routes.h"

void init_routes(Server* server) {
    server->routes = (GHashTable**)malloc(sizeof(GHashTable*) * 9);
    for (int32_t i = 0; i < 9; i++) {
        server->routes[i] = g_hash_table_new_full(g_str_hash, g_str_equal, free, _free_routes);
    }
}

void _free_routes(gpointer mem) {
    route_part* part = (route_part*)mem;
    if (part->next != NULL) g_hash_table_destroy(part->next);
    free(part); 
}

void destroy_routes(Server* server) {
    for (int32_t i = 0; i < 9; i++) {
        g_hash_table_destroy(server->routes[i]);
    }
    free(server->routes);
}

void add_new_route(Server* server, Method method, route_function function, const char* path) {
    GHashTable* dictionary = server->routes[method];
    char** parts = g_strsplit(path, "/", -1);
    // HOME
    if (parts[0] == NULL) {
        route_part* part = (route_part*)g_hash_table_lookup(dictionary, "");
        if (part == NULL) {
            part = (route_part*)malloc(sizeof(route_part));
            part->function = function;
            part->next = NULL;
            g_hash_table_insert(dictionary, g_strdup(""), part);
        }
        g_strfreev(parts);
        return;
    }
    
    int32_t i = 0;
    for (i = 0; parts[i] != NULL && parts[i + 1] != NULL; i++) {
        if (!parts[i][0] || (contains_curly_brackets(parts[i]) && g_ascii_strcasecmp(parts[i], "{arg}"))) {
            continue;
        }
        route_part* part = (route_part*)g_hash_table_lookup(dictionary, parts[i]);
        if (part == NULL) {
            part = (route_part*)malloc(sizeof(route_part));
            part->function = NULL;
            part->next = g_hash_table_new_full(g_str_hash, g_str_equal, free, _free_routes);
            g_hash_table_insert(dictionary, g_strdup(parts[i]), part);
        } else if (part->next == NULL) {
            part->next = g_hash_table_new_full(g_str_hash, g_str_equal, free, _free_routes);
        }
        dictionary = part->next;
    }
    if (parts[i][0] && !(contains_curly_brackets(parts[i]) && g_ascii_strcasecmp(parts[i], "{arg}"))) {
        
        route_part* part = (route_part*)g_hash_table_lookup(dictionary, parts[i]);
        if (part == NULL) {
            part = (route_part*)malloc(sizeof(route_part));
            part->function = function;
            part->next = NULL;
            g_hash_table_insert(dictionary, g_strdup(parts[i]), part);
        }
    }
    
    g_strfreev(parts);
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
