#include "simple_http/simple_http_app.h"

RouteHandler(t1, {    
    res->status_code = status_code.OK;
    add_response_header(res, "Content-Type", "text/html; charset=utf-8");
    g_string_append(res->body, "<!DOCTYPE html><html><head><title>Test-Title</title></head><body>Testing...1<br><a href=\"/index.html\">click me</a></body></html>");
})

int32_t main(int32_t argc, char** argv) {
    Server server;
    config_server(&server, "./src/config.txt", argc, argv);
    
    // Add routes here
    add_route(&server, METHOD_GET, "", &t1);
    
    Response* r = create_empty_response();
    add_response_header(r, "Content-Type", "text/html");
    g_string_append(r->body, "<!DOCTYPE html><html><head><title>OVERRIDE</title></head><body>X</body></html>");
    override_default_response(&server, r, status_code.NOT_FOUND, true);

    start_server(&server);
    destroy_server(&server);
    return 0;
}
