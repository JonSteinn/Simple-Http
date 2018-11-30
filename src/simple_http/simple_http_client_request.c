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