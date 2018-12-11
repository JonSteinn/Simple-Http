#include "simple_http_server_logging.h"

/**
 * Create foler and file if needed, and then open file.
 */
void init_logging(Server* server) {
    struct stat st = {0};
    if (stat("./log", &st) == -1) {
        mkdir("./log", 0700);
    }

    server->logger = fopen("./log/server-logs.log", "w");
}

/**
 * Close file.
 */
void destroy_loggging(Server* server) {
    fclose(server->logger);
}

/**
 * Write requests and response to file.
 */
void log_request(Server* server, Client* client, bool found, bool head) {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char timestamp[30];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, 30, "%a, %d %b %Y %X GMT", timeinfo);

    const char* method = head ? "HEAD" : 
                         server->request->method == METHOD_GET ? "GET" :
                         server->request->method == METHOD_POST ? "POST" : 
                         server->request->method == METHOD_PUT ? "PUT" :
                         server->request->method == METHOD_DELETE ? "DELETE" :
                         server->request->method == METHOD_CONNECTION ? "CONNECTION" :
                         server->request->method == METHOD_OPTIONS ? "OPTIONS" :
                         server->request->method == METHOD_PATCH ? "PATCH" :
                         "TRACE";

    const char* version = server->request->version == VERSION_10 ? "HTTP/1.0" :
                          server->request->version == VERSION_11 ? "HTTP/1.1" :
                          "HTTP/2.0";

    fprintf(server->logger,
            "> [%s]-[%s:%hu]-[%s]-[/%s]-[%s]-[%d]\n", 
            timestamp, 
            inet_ntoa(client->address.sin_addr), 
            client->address.sin_port,
            method,
            server->request->path->str,
            version,
            found ? server->response->status_code : 404);
}