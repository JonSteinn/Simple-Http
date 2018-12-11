# Simple HTTP
A VERY SIMPLE http server framework in C using unix socket API and glib. **This is in no way meant to be used, safe, or confine to the RFCs** (I'm way too lazy to read through that). This is just made for fun, looking under the hood and exploring the unix socket API. GLib er used for hash tables and strings.

## Video demo
TODO

## Requirments
This server is only unix and gcc compatable and requires glib2 to run.

## Run
There are bash scripts to run the program. They extend arguments to the program.
```sh
./compile.sh         # Compile only
./compile_and_run.sh # Compile, then run
./check_leaks.sh     # Compile, then run with valgrind
```

## Features
The server uses multiplexing to handle multiple clients at once. Persistent connections are timed out after a set time. From the perspective of the framework user, there are three main features.
 * Add route handling function at some route
 * Edit default responses for a given status code
 * Edit the configuration file
 * Create static content (only html, css and js are supported)

## Data structures
The only data structures that the framework user should know, are the response and request objects shown below.
```C
typedef struct {
    Version         version;
    Method          method;
    GHashTable*     queries;
    GHashTable*     headers;
    GHashTable*     cookies;
    GString*        path;
    GString*        fragment;
    GString*        body;
} Request;
```
```C
typedef struct {
    int32_t         status_code;
    GHashTable*     headers;
    GHashTable*     cookies;
    GString*        body;
} Response;
```

## Route handling
One can add a route handling method at any given route which can include parameters.

```c
RouteHandler(foo, {
  // Here you have access to the following parameters
  // Response* res  : The response you send
  // Request*  req  : The request you got
  // int32_t   argc : Number of arguments in path
  // char**    argv : The arguments in path
  
  res->status_code = status_code.OK;
  add_response_header(res, "Content-Type", "text/html; charset=utf-8");
  g_string_append(res->body, "<!DOCTYPE html><html><body>TEST</body></html>");
})
```
You can then add the handler to your server in the following way.
```C
int32_t main(int32_t argc, char** argv) {
    Server server;
    config_server(&server, "./src/config.txt", argc, argv);

    // Add routes here
    add_route(&server, METHOD_GET, "", &foo);

    start_server(&server);
    destroy_server(&server);
    return 0;
}
```
If you want parameters in your path, you can put '{arg}' instead of a word. 
```C
int32_t main(int32_t argc, char** argv) {
    Server server;
    config_server(&server, "./src/config.txt", argc, argv);

    // Add routes here
    add_route(&server, METHOD_GET, "/home/{arg}/something/{arg}", &foo);

    start_server(&server);
    destroy_server(&server);
    return 0;
}
```
Suppose the client requests the path '/home/123/something/else', then `argv[0]` would be `"123"`, `argv[1]` woulde be `"else"` and `argc` would be 2 for the handler. These parameters are always string but you are free to convert them into numbers if you please, and send a different response if you can't.

## Default response
The framework, in some cases, can send a default response before reaching a callback of a route handler. For example, if the path is not found, the default response for 404 is sent back. These can be altered by the framework user.
```C
int32_t main(int32_t argc, char** argv) {
    Server server;
    config_server(&server, "./src/config.txt", argc, argv);

    Response* r = create_empty_response();
    add_response_header(r, "Content-Type", "application/json");
    g_string_append(r->body, "{\"found\": false}");
    override_default_response(&server, r, status_code.NOT_FOUND, true);

    start_server(&server);
    destroy_server(&server);
    return 0;
}
```

## Configurations
The configuration file can be used to change certain variables for the server. The settings available are shown below with exemplary data.
```
# The name of the server
SERVER_NAME=SimpleHTTP

# Additional output
DEBUG=true

# What port to use
PORT=7000

# Max size of queue for pending connections
MAX_QUEUED=5

# Max number of clients already connected
MAX_CLIENTS=4

# Bytes read at a time from request
BUFFER_SIZE=4096

# IP address
IP=127.0.0.1

# No request poll timeout (ms)
POLL_TIMEOUT=5000

# How long can connection be open (s)
INACTIVE=10

# The max-age valeu in the header field [Cache-Control: max-age=...]
STATIC_CACHE_TIME=600
```

## Static content
The server detects any files with supported file extensions in the static folder. A filer at `statcic/index.html` is served as a response to the path '/index.html' by the client. It does check the stored routes first, so if the path is caught there, it won't serve the static content. 
