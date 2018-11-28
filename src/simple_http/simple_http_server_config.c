#include "simple_http_server_config.h"

/**
 * Constructs a new config struct. It has the priority of
 *     1) arguments 
 *     2) file 
 *     3) defaults
 * This config struc needs to be deallocated with destroy 
 * function by the caller.
 */
void init_config(server* srv, char* path, int32_t argc, char** argv) {
    srv->cfg = (config*)malloc(sizeof(config));
    GHashTable* settings = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    _config_set_default(srv->cfg);
    _config_set_from_file(settings, path);
    _config_set_from_args(settings, argc, argv);
    _config_set(srv->cfg, settings);
    
    g_hash_table_destroy(settings);

    if (srv->cfg->debug) {
        _config_display(srv->cfg);
    }
}

/**
 * Sets fields of config struct to their default values.
 */
void _config_set_default(config* cfg) {
    cfg->debug = false;
    cfg->port = 8080;
    cfg->max_queued = 100;
    cfg->max_clients = 200;
    cfg->buffer_size = 4096;
    cfg->poll_timeout = 1000;
    cfg->ip = htonl(INADDR_ANY);
    cfg->inactive_timeout = 30;
}

/**
 * Reads config files into a dictionary, splitting each line
 * on the first equal sign and places each part in the dictionary
 * as a key and value. Any line starting with '#' is considered
 * a comment and will not be parsed.
 */
void _config_set_from_file(GHashTable* settings, char* path) {
    // variables needed for reading
    FILE* file;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    file = fopen(path, "r");
    
    // If file is not found
    if (!file) {
        return;
    }

    // Read line by line
    while ((read = getline(&line, &len, file)) != EOF) {
        if (read > 2) {
            _config_parse_line(settings, line, (int32_t)read);
        }
    }

    // cleanup
    if (line) {
        free(line);
    }
    if (file) {
        fclose(file);
    }
}

/**
 * Parses each line read from the file. If it contains a newline character
 * at the end it is ignored. Also any whitespaces (or tabs and such) on
 * either side of the '=' are ignored as well as at those at the start
 * or end of the line. Each char in the key is put to uppercase. Comments
 * or invalid lines are ignored.
 */
void _config_parse_line(GHashTable* settings, char* line, int32_t chars_read) {
    // Remove prefix of ' ', '\t', '\r', etc
    while (line[0] && line[0] < 33) {
        line++;
        chars_read--;
    }

    // If too short or comment
    if (chars_read < 3 || line[0] == '#') {
        return;
    }
    
    // Any ' ', '\t', '\r', ... before '=' are ignored
    int32_t run = 0;
    for (int32_t i = 0; i < chars_read; i++) {
        if (line[i] == '=') {
            _config_split_line(settings, line, chars_read, run, i);
            break;
        } else if (line[i] < 33) {
            run++;
        } else {
            run = 0;
        }
    }
}

/**
 * Given the start of the first word and position of the delimiter ('='),
 * we calculate the length of each word, removing any pre- and postfix
 * of whitespace, tabs, etc from the second word. If both have lengths,
 * we create a copy of both, putting the first to uppercase, and place
 * in the dictionary.
 */
void _config_split_line(GHashTable* settings, char* line, int32_t chars_read, int32_t run, int32_t split) {
    // Find end of second word
    int32_t end = chars_read - 1;
    while (line[end] < 33) {
        end--;
    }

    int32_t first_size = split - run;
    int32_t second_size = end - split;
    
    // Place the split at the first char of the second word 
    // and reduce its size for each ' ', '\t', '\r',... found.
    while (line[++split] < 33 && line[split] != '\0') {
        second_size--;
    }

    if (first_size && second_size) {
        char* first = g_strndup(line, first_size);
        char* second = g_strndup(line + split, second_size);
        for (int32_t i = 0; i < first_size; i++) {
            first[i] = toupper(first[i]);
        }
        g_hash_table_insert(settings, first, second);
    }
}

/**
 * Parse command line arguments and add to dictionary. Each
 * argument is split on the first '=' and the first is put
 * to uppercase and placed as key in dictionary, the second
 * is placed as is as the corresponding value in the dictionary.
 * If splitting the argument on '=' won't yield two words, the
 * argument is ignored.
 */
void _config_set_from_args(GHashTable* settings, int32_t argc, char** argv) {
    for (int32_t i = 0; i < argc; i++) {
        char** keyval = g_strsplit(argv[i], "=", 2);
        if (keyval && keyval[0] && keyval[1]) {
            g_hash_table_insert(settings, g_ascii_strup(keyval[0], strlen(keyval[0])), g_strdup(keyval[1]));
        }
        g_strfreev(keyval);
    }
}

/**
 * Fetches values from dictionary for each field and passes it to the
 * appropriate set function. If the field is not in the dictionary,
 * NULL is passed to the set function.
 * 
 */
void _config_set(config* cfg, GHashTable* settings) {
    _config_set_debug(cfg, (char*)g_hash_table_lookup(settings, "DEBUG"));
    _config_set_port(cfg, (char*)g_hash_table_lookup(settings, "PORT"));
    _config_set_max_queued(cfg, (char*)g_hash_table_lookup(settings, "MAX_QUEUED"));
    _config_set_max_clients(cfg, (char*)g_hash_table_lookup(settings, "MAX_CLIENTS"));
    _config_set_buffer_size(cfg, (char*)g_hash_table_lookup(settings, "BUFFER_SIZE"));
    _config_set_ip(cfg, (char*)g_hash_table_lookup(settings, "IP"));
    _config_set_poll_timeout(cfg, (char*)g_hash_table_lookup(settings, "POLL_TIMEOUT"));
    _config_set_inactive_timeout(cfg, (char*)g_hash_table_lookup(settings, "INACTIVE"));
    //_config_XXX(cfg, (char*)g_hash_table_lookup(settings, "XXX"));
    //_config_XXX(cfg, (char*)g_hash_table_lookup(settings, "XXX"));
    //_config_XXX(cfg, (char*)g_hash_table_lookup(settings, "XXX"));
    //_config_XXX(cfg, (char*)g_hash_table_lookup(settings, "XXX"));
}

/**
 * Sets debug to the value in the dictionary. Does nothing if
 * value is NULL.
 */
void _config_set_debug(config* cfg, char* value) {
    if (!value) {
        return;
    }
    cfg->debug = (value[0] == 't' || value[0] == 'T' || value[0] == '1');
}

/**
 * Sets port to the value in the dictionary if valid. The string is converted 
 * to a number and stored that way. If the string is not a number, too 
 * large or a well known port, we do not set the port (and it stays as the 
 * default value). Does nothing if value is NULL.
 */
void _config_set_port(config* cfg, char* value) {
    if (!value) {
        return;
    }
    int32_t port = strtoul(value, NULL, 0);
    
    // If not number, port = 0
    // If well known, port in {1,2,...,1023}
    // if too large, port > 65535 or errno = ERANGE
    if (errno == ERANGE || port < 1024 || port > 65535) {
        return;
    }

    cfg->port = (uint16_t)port;
}

/**
 * Sets max_queued to the value in the dictionary if valid. Any positive
 * 32 bit integer is considered valid. If invalid or value is NULL, then
 * nothing gets done.
 */
void _config_set_max_queued(config* cfg, char* value) {
    if (!value) {
        return;
    }

    int32_t queue_size = strtoul(value, NULL, 0);
    if (errno == ERANGE || queue_size < 1) {
        return;
    }

    cfg->max_queued = queue_size;
}

/**
 * Sets max_clients to the value in the dictionary if valid. Any positive
 * 32 bit integer is considered valid. If invalid or value is NULL, then
 * nothing gets done.
 */
void _config_set_max_clients(config* cfg, char* value) {
    if (!value) {
        return;
    }

    int32_t max_connected = strtoul(value, NULL, 0);
    if (errno == ERANGE || max_connected < 1) {
        return;
    }

    cfg->max_clients = max_connected;

}

/**
 * Sets buffer_size to the value in the dictionary if valid. Otherwise
 * nothing is done. A valid buffer size is a positive, equal or greater
 * than 4096. 
 */
void _config_set_buffer_size(config* cfg, char* value) {
    if (!value) {
        return;
    }

    int32_t b_size = strtoul(value, NULL, 0);
    if (errno == ERANGE || b_size < 4096) {
        return;
    }

    cfg->buffer_size = b_size;
}

/**
 * Sets ip address to value in the dictionary if valid. Otherwise
 * nothing is done.
 */
void _config_set_ip(config* cfg, char* value) {
    if (!value) {
        return;
    }

    uint32_t tmp_ip;
    if (inet_pton(AF_INET, value, &tmp_ip) > 0) {
        cfg->ip = tmp_ip;
    }
}

/**
 * Sets the timeout for returing from poll when no
 * events occur, if valid. A non-positive time is invalid.
 */
void _config_set_poll_timeout(config* cfg, char* value) {
    if (!value) {
        return;
    }
    
    int32_t p_time = strtoul(value, NULL, 0);
    if (errno == ERANGE || p_time <= 0) {
        return;
    }

    cfg->poll_timeout = p_time;
}

/**
 * Sets the timeout when a keep-alive connection should be
 * closed after having been inactive, if valid.
 */
void _config_set_inactive_timeout(config* cfg, char* value) {
    if (!value) {
        return;
    }

    int32_t inact_time = strtoul(value, NULL, 0);
    if (errno == ERANGE || inact_time <= 0) {
        return;
    }

    cfg->inactive_timeout = inact_time;
}

/**
 * Display the entire configuration.
 */
void _config_display(config* cfg) {
    printf("Configuration:\n");

    // Each field
    printf("  * Debug mode: %s\n", cfg->debug ? "True" : "False");
    printf("  * Port: %hu\n", cfg->port);
    printf("  * Max queued: %d\n", cfg->max_queued);
    printf("  * Max clients: %d\n", cfg->max_clients);
    printf("  * Buffer size: %d\n", cfg->buffer_size);

    char ip_str[16];
    inet_ntop(AF_INET, &(cfg->ip), ip_str, INET_ADDRSTRLEN);
    printf("  * IP: %s\n", ip_str);

    printf("  * Poll timeout: %d\n", cfg->poll_timeout);
    printf("  * Inactive client timeout: %d\n", cfg->inactive_timeout);
    //printf("  * ", cfg->);
    
    putchar('\n');
}

/**
 * Deallocate (deep) the config struct.
 */
void destroy_config(server* srv) {
    free(srv->cfg);
}
