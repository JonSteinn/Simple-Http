#include "simple_http_server_response.h"

// 'Private' function definitions
void _append_header_to_string(gpointer key, gpointer val, gpointer data);
void _cookie_joiner(gpointer key, gpointer val, gpointer data);
void _free_default_response(gpointer mem);
void _fill_default_dictionary(GHashTable* dictionary);
void _set_default_headers(Response* response, const char* server_name);
GString* _convert_response_to_string_helper(Response* response, bool include_body);

/**
 * Initialize the single response object that the server holds
 * and is used by all clients. Must be destroyed with the function
 * destroy_reponse.
 */
void init_resposne(Server* server) {
    server->response = create_empty_response();
}

/**
 * Create an empty response object.
 */
Response* create_empty_response() {
    Response* response = (Response*)malloc(sizeof(Response));
    
    response->headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    response->body = g_string_new(NULL);
    response->status_code = 0;
    response->cookies = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    return response;
}

/**
 * Clears whatever the response object currently holds.
 */
void restart_response(Server* server) {
    g_hash_table_remove_all(server->response->headers);
    g_hash_table_remove_all(server->response->cookies);
    g_string_truncate(server->response->body, 0);
    server->response->status_code = 0;
}

/**
 * De-allocate whatever memory was allocated with init_response.
 */
void destroy_response(Server* server) {
    free_response(server->response);
}

/**
 * De-allocate response resources.
 */
void free_response(Response* response) {

    g_hash_table_destroy(response->headers);
    g_hash_table_destroy(response->cookies);
    g_string_free(response->body, true);

    free(response);
}

/**
 * Add headers to the response that the server wants to
 * handle itself. These include Content-Length (length
 * of the body), Date (time of response) and Server
 * (name of the server). If the framework user has
 * already set these headers, they are overwritten.
 */
void set_default_response_headers(Server* server) {
    _set_default_headers(server->response, server->cfg->server_name);
}

/**
 * Convert the server's response to a GString.
 */
GString* convert_response_to_string(Server* server, bool is_head) {
    return _convert_response_to_string_helper(server->response, !is_head && server->response->body->len > 0);
}

/**
 * Creates a dictionary to store default responses for each status code.
 * These can be overwritten later.
 */
void init_default_responses(Server* server) {
    server->default_response = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, _free_default_response);
    _fill_default_dictionary(server->default_response);
}

/**
 * Clean up the default responses (all).
 */
void destroy_default_responses(Server* server) {
    g_hash_table_destroy(server->default_response);
}

/**
 * Override a default response for a specific status code. If
 * free_response_memory is true, then the response is de-allocated
 * in this function. Otherwise it is left to the caller.
 */
void override_default_response(Server* server, Response* response, int32_t status_code, bool free_response_memory) {
    if (!g_ascii_strcasecmp(get_status_code_name(status_code), "Unknown")) {
        return;
    }

    response->status_code = status_code;
    _set_default_headers(response, server->cfg->server_name);
    GString* response_str = _convert_response_to_string_helper(response, response->body->len > 0);
    g_hash_table_insert(server->default_response, alloc_int(status_code), response_str);

    if (free_response_memory) {
        free_response(response);
    }
}

/////////////////////
// Private helpers //
/////////////////////

/**
 * Foreach handler that appends a single header line to the GString.
 */
void _append_header_to_string(gpointer key, gpointer val, gpointer data) {
    g_string_append_printf((GString*)data, "%s: %s\r\n", (char*)key, (char*)val);
}

/**
 * Foreach handler that joins all Set-Cookie values for a single
 * header value string.
 */
void _cookie_joiner(gpointer key, gpointer val, gpointer data) {
    char* _key = (char*)key;
    char* _val = (char*)val;
    GString* str = (GString*)data;

    if (str->len > 0) {
        g_string_append(str, "; ");
    }

    if (!g_ascii_strcasecmp(_val, "")) {
        g_string_append(str, _key);
    } else {
        g_string_append_printf(str, "%s=%s", _key, _val);
    }
}

/**
 * Memory de-allocation function for the default response. 
 */
void _free_default_response(gpointer mem) {
    g_string_free((GString*)mem, true);
}

/**
 * Add default headers to given response.
 */
void _set_default_headers(Response* response, const char* server_name) {

    // Conrent-Length
    GString* tmp_str = g_string_new(NULL);
    g_string_append_printf(tmp_str, "%zu", response->body->len);
    g_hash_table_insert(response->headers, g_strdup("Content-Length"), g_strdup(tmp_str->str));
    g_string_free(tmp_str, true);

    // Date
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char timestamp[30];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, 30, "%a, %d %b %Y %X GMT", timeinfo);
    g_hash_table_insert(response->headers, g_strdup("Date"), g_strdup(timestamp));

    // Server
    g_hash_table_insert(response->headers, g_strdup("Server"), g_strdup(server_name));
}

/**
 * Helper to convert any response to a GString.
 */
GString* _convert_response_to_string_helper(Response* response, bool include_body) {
    GString* str = g_string_new_len(NULL, 1000);

    // Start line
    g_string_append_printf(str, "HTTP/1.1 %d %s\r\n", response->status_code, 
        get_status_code_name(response->status_code));

    // Headers (excluding Set-Cookie)
    g_hash_table_foreach(response->headers, _append_header_to_string, str);

    // Cookies
    GString* cookie_tmp_str = g_string_new(NULL);
    g_hash_table_foreach(response->cookies, _cookie_joiner, cookie_tmp_str);
    if (cookie_tmp_str->len > 0) {
        g_string_append_printf(str, "Set-Cookie: %s\r\n", cookie_tmp_str->str);
    }
    g_string_free(cookie_tmp_str, true);

    // Blank line
    g_string_append(str, "\r\n");

    // Body
    if (include_body) {
        g_string_append(str, response->body->str);
    }

    return str;
}

/**
 * Fill the default dictionary.
 */
void _fill_default_dictionary(GHashTable* dictionary) {
    g_hash_table_insert(dictionary, alloc_int(100), g_string_new(
        "HTTP/1.1 100 Continue\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 93\r\n\r\n"
        "<!DOCTYPE html><html><head><title>100 Continue</title></head><body>100 Continue</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(101), g_string_new(
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 115\r\n\r\n"
        "<!DOCTYPE html><html><head><title>101 Switching Protocols</title></head><body>101 Switching Protocols</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(102), g_string_new(
        "HTTP/1.1 102 Processing\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 97\r\n\r\n"
        "<!DOCTYPE html><html><head><title>102 Processing</title></head><body>102 Processing</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(103), g_string_new(
        "HTTP/1.1 103 Early Hints\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 99\r\n\r\n"
        "<!DOCTYPE html><html><head><title>103 Early Hints</title></head><body>103 Early Hints</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(200), g_string_new(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 81\r\n\r\n"
        "<!DOCTYPE html><html><head><title>200 OK</title></head><body>200 OK</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(201), g_string_new(
        "HTTP/1.1 201 Created\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 91\r\n\r\n"
        "<!DOCTYPE html><html><head><title>201 Created</title></head><body>201 Created</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(202), g_string_new(
        "HTTP/1.1 202 Accepted\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 93\r\n\r\n"
        "<!DOCTYPE html><html><head><title>202 Accepted</title></head><body>202 Accepted</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(203), g_string_new(
        "HTTP/1.1 203 Non-Authoritative Information\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 135\r\n\r\n"
        "<!DOCTYPE html><html><head><title>203 Non-Authoritative Information</title></head><body>203 Non-Authoritative Information</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(204), g_string_new(
        "HTTP/1.1 204 No Content\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 97\r\n\r\n"
        "<!DOCTYPE html><html><head><title>204 No Content</title></head><body>204 No Content</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(205), g_string_new(
        "HTTP/1.1 205 Reset Content\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 103\r\n\r\n"
        "<!DOCTYPE html><html><head><title>205 Reset Content</title></head><body>205 Reset Content</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(206), g_string_new(
        "HTTP/1.1 206 Partial Content\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 107\r\n\r\n"
        "<!DOCTYPE html><html><head><title>206 Partial Content</title></head><body>206 Partial Content</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(207), g_string_new(
        "HTTP/1.1 207 Multi-Status\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 101\r\n\r\n"
        "<!DOCTYPE html><html><head><title>207 Multi-Status</title></head><body>207 Multi-Status</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(208), g_string_new(
        "HTTP/1.1 208 Already Reported\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 109\r\n\r\n"
        "<!DOCTYPE html><html><head><title>208 Already Reported</title></head><body>208 Already Reported</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(226), g_string_new(
        "HTTP/1.1 226 IM Used\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 91\r\n\r\n"
        "<!DOCTYPE html><html><head><title>226 IM Used</title></head><body>226 IM Used</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(300), g_string_new(
        "HTTP/1.1 300 Multiple Choices\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 109\r\n\r\n"
        "<!DOCTYPE html><html><head><title>300 Multiple Choices</title></head><body>300 Multiple Choices</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(301), g_string_new(
        "HTTP/1.1 301 Moved Permanently\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 111\r\n\r\n"
        "<!DOCTYPE html><html><head><title>301 Moved Permanently</title></head><body>301 Moved Permanently</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(302), g_string_new(
        "HTTP/1.1 302 Found\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 87\r\n\r\n"
        "<!DOCTYPE html><html><head><title>302 Found</title></head><body>302 Found</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(303), g_string_new(
        "HTTP/1.1 303 See Other\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 95\r\n\r\n"
        "<!DOCTYPE html><html><head><title>303 See Other</title></head><body>303 See Other</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(304), g_string_new(
        "HTTP/1.1 304 Not Modified\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 101\r\n\r\n"
        "<!DOCTYPE html><html><head><title>304 Not Modified</title></head><body>304 Not Modified</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(305), g_string_new(
        "HTTP/1.1 305 Use Proxy\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 95\r\n\r\n"
        "<!DOCTYPE html><html><head><title>305 Use Proxy</title></head><body>305 Use Proxy</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(306), g_string_new(
        "HTTP/1.1 306 Switch Proxy\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 101\r\n\r\n"
        "<!DOCTYPE html><html><head><title>306 Switch Proxy</title></head><body>306 Switch Proxy</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(307), g_string_new(
        "HTTP/1.1 307 Temporary Redirect\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 113\r\n\r\n"
        "<!DOCTYPE html><html><head><title>307 Temporary Redirect</title></head><body>307 Temporary Redirect</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(308), g_string_new(
        "HTTP/1.1 308 Permanent Redirect\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 113\r\n\r\n"
        "<!DOCTYPE html><html><head><title>308 Permanent Redirect</title></head><body>308 Permanent Redirect</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(400), g_string_new(
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 99\r\n\r\n"
        "<!DOCTYPE html><html><head><title>400 Bad Request</title></head><body>400 Bad Request</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(401), g_string_new(
        "HTTP/1.1 401 Unauthorized\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 101\r\n\r\n"
        "<!DOCTYPE html><html><head><title>401 Unauthorized</title></head><body>401 Unauthorized</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(402), g_string_new(
        "HTTP/1.1 402 Payment Required\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 109\r\n\r\n"
        "<!DOCTYPE html><html><head><title>402 Payment Required</title></head><body>402 Payment Required</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(403), g_string_new(
        "HTTP/1.1 403 Forbidden\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 95\r\n\r\n"
        "<!DOCTYPE html><html><head><title>403 Forbidden</title></head><body>403 Forbidden</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(404), g_string_new(
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 95\r\n\r\n"
        "<!DOCTYPE html><html><head><title>404 Not Found</title></head><body>404 Not Found</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(405), g_string_new(
        "HTTP/1.1 405 Method Not Allowed\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 113\r\n\r\n"
        "<!DOCTYPE html><html><head><title>405 Method Not Allowed</title></head><body>405 Method Not Allowed</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(406), g_string_new(
        "HTTP/1.1 406 Not Acceptable\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 105\r\n\r\n"
        "<!DOCTYPE html><html><head><title>406 Not Acceptable</title></head><body>406 Not Acceptable</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(407), g_string_new(
        "HTTP/1.1 407 Proxy Authentication Required\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 135\r\n\r\n"
        "<!DOCTYPE html><html><head><title>407 Proxy Authentication Required</title></head><body>407 Proxy Authentication Required</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(408), g_string_new(
        "HTTP/1.1 408 Request Timeout\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 107\r\n\r\n"
        "<!DOCTYPE html><html><head><title>408 Request Timeout</title></head><body>408 Request Timeout</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(409), g_string_new(
        "HTTP/1.1 409 Conflict\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 93\r\n\r\n"
        "<!DOCTYPE html><html><head><title>409 Conflict</title></head><body>409 Conflict</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(410), g_string_new(
        "HTTP/1.1 410 Gone\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 85\r\n\r\n"
        "<!DOCTYPE html><html><head><title>410 Gone</title></head><body>410 Gone</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(411), g_string_new(
        "HTTP/1.1 411 Length Required\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 107\r\n\r\n"
        "<!DOCTYPE html><html><head><title>411 Length Required</title></head><body>411 Length Required</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(412), g_string_new(
        "HTTP/1.1 412 Precondition Failed\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 115\r\n\r\n"
        "<!DOCTYPE html><html><head><title>412 Precondition Failed</title></head><body>412 Precondition Failed</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(413), g_string_new(
        "HTTP/1.1 413 Payload Too Large\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 111\r\n\r\n"
        "<!DOCTYPE html><html><head><title>413 Payload Too Large</title></head><body>413 Payload Too Large</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(414), g_string_new(
        "HTTP/1.1 414 URI Too Long\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 101\r\n\r\n"
        "<!DOCTYPE html><html><head><title>414 URI Too Long</title></head><body>414 URI Too Long</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(415), g_string_new(
        "HTTP/1.1 415 Unsupported Media Type\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 121\r\n\r\n"
        "<!DOCTYPE html><html><head><title>415 Unsupported Media Type</title></head><body>415 Unsupported Media Type</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(416), g_string_new(
        "HTTP/1.1 416 Range Not Satisfiable\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 119\r\n\r\n"
        "<!DOCTYPE html><html><head><title>416 Range Not Satisfiable</title></head><body>416 Range Not Satisfiable</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(417), g_string_new(
        "HTTP/1.1 417 Expectation Failed\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 113\r\n\r\n"
        "<!DOCTYPE html><html><head><title>417 Expectation Failed</title></head><body>417 Expectation Failed</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(418), g_string_new(
        "HTTP/1.1 418 I'm a teapot\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 101\r\n\r\n"
        "<!DOCTYPE html><html><head><title>418 I'm a teapot</title></head><body>418 I'm a teapot</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(421), g_string_new(
        "HTTP/1.1 421 Misdirected Request\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 115\r\n\r\n"
        "<!DOCTYPE html><html><head><title>421 Misdirected Request</title></head><body>421 Misdirected Request</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(422), g_string_new(
        "HTTP/1.1 422 Unprocessable Entity\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 117\r\n\r\n"
        "<!DOCTYPE html><html><head><title>422 Unprocessable Entity</title></head><body>422 Unprocessable Entity</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(423), g_string_new(
        "HTTP/1.1 423 Locked\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 89\r\n\r\n"
        "<!DOCTYPE html><html><head><title>423 Locked</title></head><body>423 Locked</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(424), g_string_new(
        "HTTP/1.1 424 Failed Dependency\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 111\r\n\r\n"
        "<!DOCTYPE html><html><head><title>424 Failed Dependency</title></head><body>424 Failed Dependency</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(426), g_string_new(
        "HTTP/1.1 426 Upgrade Required\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 109\r\n\r\n"
        "<!DOCTYPE html><html><head><title>426 Upgrade Required</title></head><body>426 Upgrade Required</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(428), g_string_new(
        "HTTP/1.1 428 Precondition Required\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 119\r\n\r\n"
        "<!DOCTYPE html><html><head><title>428 Precondition Required</title></head><body>428 Precondition Required</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(429), g_string_new(
        "HTTP/1.1 429 Too Many Requests\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 111\r\n\r\n"
        "<!DOCTYPE html><html><head><title>429 Too Many Requests</title></head><body>429 Too Many Requests</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(431), g_string_new(
        "HTTP/1.1 431 Request Header Fields Too Large\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 139\r\n\r\n"
        "<!DOCTYPE html><html><head><title>431 Request Header Fields Too Large</title></head><body>431 Request Header Fields Too Large</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(451), g_string_new(
        "HTTP/1.1 451 Unavailable For Legal Reasons\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 135\r\n\r\n"
        "<!DOCTYPE html><html><head><title>451 Unavailable For Legal Reasons</title></head><body>451 Unavailable For Legal Reasons</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(500), g_string_new(
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 119\r\n\r\n"
        "<!DOCTYPE html><html><head><title>500 Internal Server Error</title></head><body>500 Internal Server Error</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(501), g_string_new(
        "HTTP/1.1 501 Not Implemented\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 107\r\n\r\n"
        "<!DOCTYPE html><html><head><title>501 Not Implemented</title></head><body>501 Not Implemented</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(502), g_string_new(
        "HTTP/1.1 502 Bad Gateway\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 99\r\n\r\n"
        "<!DOCTYPE html><html><head><title>502 Bad Gateway</title></head><body>502 Bad Gateway</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(503), g_string_new(
        "HTTP/1.1 503 Service Unavailable\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 115\r\n\r\n"
        "<!DOCTYPE html><html><head><title>503 Service Unavailable</title></head><body>503 Service Unavailable</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(504), g_string_new(
        "HTTP/1.1 504 Gateway Timeout\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 107\r\n\r\n"
        "<!DOCTYPE html><html><head><title>504 Gateway Timeout</title></head><body>504 Gateway Timeout</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(505), g_string_new(
        "HTTP/1.1 505 HTTP Version Not Supported\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 129\r\n\r\n"
        "<!DOCTYPE html><html><head><title>505 HTTP Version Not Supported</title></head><body>505 HTTP Version Not Supported</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(506), g_string_new(
        "HTTP/1.1 506 Variant Also Negotiates\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 123\r\n\r\n"
        "<!DOCTYPE html><html><head><title>506 Variant Also Negotiates</title></head><body>506 Variant Also Negotiates</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(507), g_string_new(
        "HTTP/1.1 507 Insufficient Storage\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 117\r\n\r\n"
        "<!DOCTYPE html><html><head><title>507 Insufficient Storage</title></head><body>507 Insufficient Storage</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(508), g_string_new(
        "HTTP/1.1 508 Loop Detected\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 103\r\n\r\n"
        "<!DOCTYPE html><html><head><title>508 Loop Detected</title></head><body>508 Loop Detected</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(510), g_string_new(
        "HTTP/1.1 510 Not Extended\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 101\r\n\r\n"
        "<!DOCTYPE html><html><head><title>510 Not Extended</title></head><body>510 Not Extended</body></html>"
    ));

    g_hash_table_insert(dictionary, alloc_int(511), g_string_new(
        "HTTP/1.1 511 Network Authentication Required\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Length: 139\r\n\r\n"
        "<!DOCTYPE html><html><head><title>511 Network Authentication Required</title></head><body>511 Network Authentication Required</body></html>"
    ));
}

