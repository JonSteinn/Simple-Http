#include "simple_http_client_request.h"

/**
 * Initialize the single request object that the server
 * holds. This requires a call of destroy_request to
 * free the memory allocted in this function.
 */
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

/**
 * Resets the request after it has been used.
 */
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

/**
 * De-allocate the memory for the server's request object.
 */
void destroy_request(Server* server) {
    g_hash_table_destroy(server->request->queries);
    g_hash_table_destroy(server->request->headers);
    g_hash_table_destroy(server->request->cookies);
    g_string_free(server->request->path, true);
    g_string_free(server->request->fragment, true);
    g_string_free(server->request->body, true);
    free(server->request);
}

/**
 * Try to parse a raw string from a client into a request object.
 * If successful, true is returned, othwerise false.
 */
bool parse_request(Server* server, GString* raw_request) {
    char** empty_line_split = g_strsplit(raw_request->str, "\r\n\r\n", 2);

    // If missing start line or no occurrence of "\r\n\r\n"
    if (!empty_line_split[0] || empty_line_split[0][0] == '\0' || !empty_line_split[1]) {
        g_strfreev(empty_line_split);
        g_string_truncate(raw_request, 0);
        return false;
    }

    // Start line + headers
    char** lines = g_strsplit(empty_line_split[0], "\r\n", -1);
    
    bool start_line_is_valid = _parse_start_line(server, lines[0]);
    
    if (start_line_is_valid) {
        _parse_headers(server, lines);

        // If contains a body
        if (empty_line_split[1][0] != '\0') {
            g_string_append(server->request->body, empty_line_split[1]);
        }
    }

    // Free resources and clear raw_request
    g_strfreev(lines);
    g_strfreev(empty_line_split);
    g_string_truncate(raw_request, 0);

    return true;
}

/**
 * Parse the start line. It should contain method, url and version,
 * in that order and seperated by a whitespace. If the startline is
 * valid we return true, otherwise false.
 */
bool _parse_start_line(Server* server, char* line) {
    char** start_line = g_strsplit(line, " ", 3);    

    // If contain three non-empty words and each word can be parsed.
    bool valid = start_line[0] != NULL && start_line[0][0] != '\0' &&
                 start_line[1] != NULL && start_line[1][0] != '\0' &&
                 start_line[2] != NULL && start_line[2][0] != '\0' &&
                 _set_http_method(server, start_line[0]) &&
                 _parse_url(server, start_line[1]) &&
                 _set_http_version(server, start_line[2]);

    g_strfreev(start_line);

    return valid;
}

/**
 * If method matches a valid http method, we set
 * the request method to the corresponding enum
 * and return true. Otherwise false is returned.
 */
bool _set_http_method(Server* server, char* method) {
    if (!g_ascii_strncasecmp("GET", method, 3)) {
        server->request->method = METHOD_GET;
    } else if (!g_ascii_strncasecmp("HEAD", method, 4)) {
        server->request->method = METHOD_HEAD;
    } else if (!g_ascii_strncasecmp("POST", method, 4)) {
        server->request->method = METHOD_POST;
    } else if (!g_ascii_strncasecmp("PUT", method, 3)) {
        server->request->method = METHOD_PUT;
    } else if (!g_ascii_strncasecmp("DELETE", method, 6)) {
        server->request->method = METHOD_DELETE;
    } else if (!g_ascii_strncasecmp("CONNECTION", method, 10)) {
        server->request->method = METHOD_CONNECTION;
    } else if (!g_ascii_strncasecmp("OPTIONS", method, 7)) {
        server->request->method = METHOD_OPTIONS;
    } else if (!g_ascii_strncasecmp("TRACE", method, 5)) {
        server->request->method = METHOD_TRACE;
    } else if (!g_ascii_strncasecmp("PATCH", method, 5)) {
        server->request->method = METHOD_PATCH;
    } else {
        return false;
    }
    return true;
}

/**
 * Parse url and return true if valid, otherwise false.
 * The url may not contain '{', '}', '|', '\', '^', '[', 
 * ']', '`', '<', '>'. It can (optionally) have both a
 * query string (after ?) and/or fragment (after #).
 * The query string is parsed as well.
 */
bool _parse_url(Server* server, char* url) {
    char** first_split = NULL;
    char** second_split = NULL;
    first_split = g_strsplit(url, "?", 2);


    if (first_split[1] == NULL) {
        // If url has no query:
        // Option i:  <path>#<fragment>
        // Option ii: <path>

        // split into <path> and <fragment>
        second_split = g_strsplit(url, "#", 2);

        size_t len = string_length_without_trailing_forward_slash(second_split[0]);
        size_t offset = string_offset_jumping_leading_forward_slash(second_split[0], len);
        g_string_append_len(server->request->path, second_split[0] + offset, len - offset);
        
        // If option i and the fragment is not the empty word.
        if (second_split[1] != NULL && second_split[1][0] != '\0') {
            g_string_append(server->request->fragment, second_split[1]);
        }
    } else {
        // If url has query
        // Option i:  <path>?<queries>#<fragment>
        // Option ii: <path>?<queries>

        // We already have the path in the first split
        size_t len = string_length_without_trailing_forward_slash(first_split[0]);
        size_t offset = string_offset_jumping_leading_forward_slash(first_split[0], len);
        g_string_append_len(server->request->path, first_split[0] + offset, len - offset);

        // Split remaining (string after ?) into <queries> and <fragment>
        second_split = g_strsplit(first_split[1], "#", 2);

        if (second_split[0][0] != '\0') {
            // if queries is not the empty word
            _parse_queries(server, second_split[0]);
        }
        if (second_split[1] != NULL && second_split[1][0] != '\0') {
            // If the fragment exist and is not the empty word
            g_string_append(server->request->fragment, second_split[1]);
        }
    }

    g_strfreev(first_split);
    g_strfreev(second_split);

    // Check for: {, }, |, \, ^, [, ], `, <, >
    for (size_t i = 0; i < server->request->path->len; i++) {
        if (server->request->path->str[i] == '}' ||
            server->request->path->str[i] == '{' ||
            server->request->path->str[i] == '|' ||
            server->request->path->str[i] == '\\' ||
            server->request->path->str[i] == '[' ||
            server->request->path->str[i] == ']' ||
            server->request->path->str[i] == '`' ||
            server->request->path->str[i] == '<' ||
            server->request->path->str[i] == '>') {
                
            return false;
        }
    }

    return true;
}

/**
 * See if version matches the valid htpp versions and if so
 * set the version of the request and return true, otherwise
 * false is returned.
 */
bool _set_http_version(Server* server, char* version) {
    if (!g_ascii_strncasecmp("HTTP/1.0", version, 8)) {
        server->request->version = VERSION_10;
    } else if (!g_ascii_strncasecmp("HTTP/1.1", version, 8)) {
        server->request->version = VERSION_11;
    } else if (!g_ascii_strncasecmp("HTTP/2.0", version, 8)) {
        server->request->version = VERSION_20;
    } else {
        return false;
    }

    return true;
}

/**
 * Parse queries into a dictionary. The queries must be
 * seperated by a '&' and each query must consist of a
 * key and value seperated by a '='.
 */
void _parse_queries(Server* server, char* query_string) {
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

/**
 * Parse headers. A single header must be within its own line.
 * It must be of the form "<key>: <value>\r\n". Any header
 * with the key "Cookie" placed elsewhere (then the request's
 * header dictionary).
 */
void _parse_headers(Server* server, char** lines) {
    for (int32_t i = 1; lines[i] != NULL; i++) {
        char** header = g_strsplit(lines[i], ": ", 2);
        if (header[0] != NULL && header[0][0] != '\0' && header[1] != NULL && header[1][0] != '\0') {
            if (!g_ascii_strncasecmp("Cookie", header[0], 6)) {
                _parse_cookie(server, header[1]);
            } else {
                g_hash_table_insert(server->request->headers, g_strdup(header[0]), g_strdup(header[1]));
            }
        }
        g_strfreev(header);
    }
}

/**
 * Parse cookies. Each cookie must seperated by a ';'. White spaces
 * at the end of cookies are ignored so both "<cookie1>;<cookie2>;..."
 * and "<cookie1>; <cookie2>; ..." are fine. Cookies can be a key-value
 * pair or just a single value, The key-value pairs are placed that way
 * into a dictionary while a single value added to the dictionary such
 * that it maps to the empty word.
 */
void _parse_cookie(Server* server, char* cookie) {
    char** cookies = g_strsplit(cookie, ";", -1);
    for (int32_t i = 0; cookies[i] != NULL; i++) {
        if (cookies[i][0] != '\0') {
            char** keyval = g_strsplit(cookies[i], "=", 2);
            
            char* key = trim_end_white_spaces_alloc(keyval[0]);
            if (key != NULL) {
                if (keyval[1] == NULL) {
                    g_hash_table_insert(server->request->cookies, key, g_strdup(""));
                } else {
                    char* val = trim_end_white_spaces_alloc(keyval[1]);
                    if (val != NULL) {
                        g_hash_table_insert(server->request->cookies, key, val);
                    } else {
                        free(key);
                    }
                }
            }

            g_strfreev(keyval);
        }
    }
    g_strfreev(cookies);
}

/**
 * Check if the connection is persistent.
 */
bool keep_alive(Server* server) {
    char* connection = g_hash_table_lookup(server->request->headers, "Connection");

    if (server->request->version == VERSION_10) {
        return connection != NULL && !g_ascii_strcasecmp(connection, "keep-alive");
    } else {
        return connection == NULL || g_ascii_strcasecmp(connection, "close");
    }
}
