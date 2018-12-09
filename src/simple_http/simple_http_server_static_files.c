#include "simple_http_server_static_files.h"

void init_static_files(Server* server) {
    server->static_files = (StaticFiles*)malloc(sizeof(StaticFiles));

    DIR* dir = opendir("./src/static");
    if (!dir) {
        server->static_files->has_static = false;
        return;
    }
    server->static_files->has_static = true;
    _add_supported_media_types(server); 
    server->static_files->files = g_hash_table_new_full(g_str_hash, g_str_equal, free, _no_free);
    _find_static_files_recursive(dir, "./src/static", server->static_files);
    closedir(dir);
}

void _add_supported_media_types(Server* server) {
    server->static_files->supported_media_types = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    g_hash_table_insert(server->static_files->supported_media_types, g_strdup("html"), g_strdup("text/html; charset=utf-8"));
    g_hash_table_insert(server->static_files->supported_media_types, g_strdup("js"), g_strdup("application/javascript; charset=utf-8"));
    g_hash_table_insert(server->static_files->supported_media_types, g_strdup("css"), g_strdup("text/css; charset=utf-8"));
    g_hash_table_insert(server->static_files->supported_media_types, g_strdup("png"), g_strdup("image/png"));
    g_hash_table_insert(server->static_files->supported_media_types, g_strdup("jpg"), g_strdup("image/jpeg"));
    g_hash_table_insert(server->static_files->supported_media_types, g_strdup("ico"), g_strdup("image/x-icon"));
}

void _no_free(__attribute__((unused)) gpointer mem) { }

void _find_static_files_recursive(DIR* dir, const char* prefix, StaticFiles* static_files) {
    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_type == 8) {
            GString* tmp = g_string_new(prefix);
            g_string_append_printf(tmp, "/%s", dp->d_name);
            if (_valid_extension(tmp, static_files->supported_media_types)) {
                g_hash_table_insert(static_files->files, g_strdup(tmp->str + 13), &static_files->has_static);
            }
            g_string_free(tmp, true);
        } else if (dp->d_type == 4 && dp->d_name[0] != '.') {
            GString* tmp_prefix = g_string_new(prefix);
            g_string_append_printf(tmp_prefix, "/%s", dp->d_name);            
            DIR* tmp_dir = opendir(tmp_prefix->str);
            if (tmp_dir) {
                _find_static_files_recursive(tmp_dir, tmp_prefix->str, static_files);
            }
            g_string_free(tmp_prefix, true);
            closedir(tmp_dir);
        }
    }
}

bool _valid_extension(GString* file, GHashTable* supported) {
    size_t i = 0;
    for (i = file->len - 1; ; i--) {
        if (file->str[i] == '.') {
            if (i == file->len - 1) {
                return false;
            } else {
                char* extension = g_strdup(file->str + (i+1));
                bool valid = g_hash_table_lookup(supported, extension) != NULL;
                free(extension);
                return valid;
            }
        } else if (file->str[i] == '/') {
            return false;
        }
        if (i == 0) {
            break;
        }
    }
    return false;
}

void destroy_static_files(Server* server) {
    if (server->static_files->has_static) {
        g_hash_table_destroy(server->static_files->supported_media_types);
        g_hash_table_destroy(server->static_files->files);
    }
    free(server->static_files);
}




bool read_file_into_response(Server* server) {    
    if (server->request->method != METHOD_GET || g_hash_table_lookup(server->static_files->files, server->request->path->str) == NULL) {
        return false;
    }

    // Path from src folder
    GString* full_path = _construct_full_path(server->request->path->str);

    // Open file
    FILE* file = fopen(full_path->str, "r");
    if (!file) {
        g_string_free(full_path, true);
        return false;
    }

    // Get memtype
    char* mem_type = NULL;
    for (size_t i = server->request->path->len - 1; ; i--) {
        if (server->request->path->str[i] == '.') {
            if (i < server->request->path->len - 1) {
                mem_type = (char*)g_hash_table_lookup(server->static_files->supported_media_types, server->request->path->str + (i+1));
            }
            break;
        }
        if (i == 0) {
            break;
        }
    }

    // If invalid memtype
    if (mem_type == NULL) {
        g_string_free(full_path, true);
        return false;
    }

    // Read file into body
    size_t len = 0;
    ssize_t read;
    char* line = NULL;
    while ((read = getline(&line, &len, file)) != EOF) {
        g_string_append(server->response->body, line);
    }

    // Add static file standard headers
    _add_file_headers(server, mem_type, full_path->str);

    // cleanup
    if (line) {
        free(line);
    }
    if (file) {
        fclose(file);
    }
    g_string_free(full_path, true);

    return true;
}

GString* _construct_full_path(const char* file_path) {
    GString* full_path = g_string_new("./src/static/");
    g_string_append(full_path, file_path);
    return full_path;
}

void _add_file_headers(Server* server, const char* mem_type, const char* full_path) {
    g_hash_table_insert(server->response->headers, g_strdup("Content-Type"), g_strdup(mem_type));

    GString* max_age = g_string_new_len("max-age=", 20);
    g_string_printf(max_age, "%d", server->cfg->cache_time);
    g_hash_table_insert(server->response->headers, g_strdup("Cache-Control"), g_strdup(max_age->str));
    g_string_free(max_age, true);

    struct stat statbuf;
    if (!stat(full_path, &statbuf)) {
        time_t t = statbuf.st_mtime;
        struct tm* timeinfo;
        timeinfo = localtime(&t);
        char timestamp[30];
        memset(timestamp, 0, sizeof(timestamp));
        strftime(timestamp, 30, "%a, %d %b %Y %X GMT", timeinfo);
        g_hash_table_insert(server->response->headers, g_strdup("Last-Modified"), g_strdup(timestamp));
    }
}



/* STAT:

            

*/