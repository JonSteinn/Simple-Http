#include "simple_http_client_request.h"


void init_request(Server* server) {
    server->request = (Request*)malloc(sizeof(Request));

    server->request->version = VERSION_11;
    server->request->method = METHOD_GET;
    server->request->queries = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    server->request->headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    server->request->cookies = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    server->request->path = g_string_new(NULL);
    server->request->fragment = g_string_new(NULL);
    server->request->body = g_string_new(NULL);
}

void restart_request(Server* server) {
    server->request->version = VERSION_11;
    server->request->method = METHOD_GET;
    g_hash_table_remove_all(server->request->queries);
    g_hash_table_remove_all(server->request->headers);
    g_hash_table_remove_all(server->request->cookies);
    g_string_truncate(server->request->path, 0);
    g_string_truncate(server->request->fragment, 0);
    g_string_truncate(server->request->body, 0);
}

void destroy_request(Server* server) {
    g_hash_table_destroy(server->request->queries);
    g_hash_table_destroy(server->request->headers);
    g_hash_table_destroy(server->request->cookies);
    g_string_free(server->request->path, true);
    g_string_free(server->request->fragment, true);
    g_string_free(server->request->body, true);
    free(server->request);
}









bool set_http_version(Server* server, char* version) {
    if (g_ascii_strncasecmp("HTTP/1.0", version, 8)) {
        server->request->version = VERSION_10;
    } else if (g_ascii_strncasecmp("HTTP/1.1", version, 8)) {
        server->request->version = VERSION_11;
    } else if (g_ascii_strncasecmp("HTTP/2.0", version, 8)) {
        server->request->version = VERSION_20;
    } else {
        return false;
    }

    return true;
}

void parse_queries(Server* server, char* query_string) {
    char** queries = g_strsplit(query_string, "&", -1);
    for (int32_t i = 0; queries[i] != NULL; i++) {
        char** query = g_strsplit(queries[i], "=", 2);
        if (query[0] != NULL && query[0][0] != '\0' && query[1] != NULL && query[1][0] != '\0') {                    
            g_hash_table_insert(server->request->queries, g_strdup(query[0]), g_strdup(query[1]));
        }
        g_strfreev(query);
    }
    g_strfreev(queries);
}

bool parse_url(Server* server, char* url) {
    char** first_split = NULL;
    char** second_split = NULL;

    first_split = g_strsplit(url, "?", 2);
    if (first_split[1] == NULL) {
        // Has no query
        second_split = g_strsplit(url, "#", 2);
        g_string_append(server->request->path, second_split[0]);
        if (second_split[1] != NULL && second_split[1][0] != '\0') {
            g_string_append(server->request->fragment, second_split[1]);
        }
    } else {
        // Has query
        g_string_append(server->request->path, first_split[0]);
        second_split = g_strsplit(first_split[1], "#", 2);
        if (second_split[0][0] != '\0') {
            parse_queries(server, second_split[0]);
        }
        if (second_split[1] != NULL && second_split[1][0] != '\0') {
            g_string_append(server->request->fragment, second_split[1]);
        }
    }

    g_strfreev(first_split);
    g_strfreev(second_split);


    // TODO: validate url ? 
    return true;
}


bool set_http_method(Server* server, char* method) {
    if (g_ascii_strncasecmp("GET", method, 3)) {
        server->request->method = METHOD_GET;
    } else if (g_ascii_strncasecmp("HEAD", method, 4)) {
        server->request->method = METHOD_HEAD;
    } else if (g_ascii_strncasecmp("POST", method, 4)) {
        server->request->method = METHOD_POST;
    } else if (g_ascii_strncasecmp("PUT", method, 3)) {
        server->request->method = METHOD_PUT;
    } else if (g_ascii_strncasecmp("DELETE", method, 6)) {
        server->request->method = METHOD_DELETE;
    } else if (g_ascii_strncasecmp("CONNECTION", method, 10)) {
        server->request->method = METHOD_CONNECTION;
    } else if (g_ascii_strncasecmp("OPTIONS", method, 7)) {
        server->request->method = METHOD_OPTIONS;
    } else if (g_ascii_strncasecmp("TRACE", method, 5)) {
        server->request->method = METHOD_TRACE;
    } else if (g_ascii_strncasecmp("PATCH", method, 5)) {
        server->request->method = METHOD_PATCH;
    } else {
        return false;
    }

    return true;
}

bool parse_start_line(Server* server, char* line) {
    char** start_line = g_strsplit(line, " ", 3);    

    bool valid = start_line[0] != NULL && start_line[0][0] != '\0' &&
                 start_line[1] != NULL && start_line[1][0] != '\0' &&
                 start_line[2] != NULL && start_line[2][0] != '\0' &&
                 set_http_method(server, start_line[0]) &&
                 parse_url(server, start_line[1]) &&
                 set_http_version(server, start_line[2]);

    g_strfreev(start_line);

    return valid;
}

bool parse_request(Server* server, GString* raw_request) {
    char** empty_line_split = g_strsplit(raw_request->str, "\r\n\r\n", 2);
    if (!empty_line_split[0] || empty_line_split[0][0] == '\0' || !empty_line_split[1]) {
        g_strfreev(empty_line_split);
        return false;
    }

    char** lines = g_strsplit(empty_line_split[0], "\r\n", -1);

    bool valid = parse_start_line(server, lines[0]);

    if (valid) {

        // Headers
        for (int32_t i = 1; lines[i] != NULL; i++) {
            char** header = g_strsplit(lines[i], ": ", 2);
            if (header[0] != NULL && header[0][0] != '\0' && header[1] != NULL && header[1][0] != '\0') {
                // TODO Handle cookies ...
                g_hash_table_insert(server->request->headers, g_strdup(header[0]), g_strdup(header[1]));
            }
            g_strfreev(header);
        }


        // Body 
        if (empty_line_split[1][0] != '\0') {
            g_string_append(server->request->body, empty_line_split[1]);
        }
    }


    g_strfreev(lines);
    g_strfreev(empty_line_split);

    return true;
}