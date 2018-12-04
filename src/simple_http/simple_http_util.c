#include "simple_http_util.h"

/**
 * Allocate memory and set it to the given integer,
 * and return a pointer to it.
 */
int32_t* alloc_int(int32_t integer) {
    int32_t* new_int = (int32_t*)malloc(sizeof(int32_t));
    *new_int = integer;
    return new_int;
}

/**
 * Copy a given string but without any leading or 
 * trailing whitespaces.
 */
char* trim_end_white_spaces_alloc(const char* str) {
    size_t len = strlen(str);

    // Trailing
    while (len > 0 && str[len - 1] == ' ') {
        len--;
    }

    // Leading
    size_t start = 0;
    while (start < len && str[start] == ' ') {
        start++;
    }

    return len == 0 ? NULL : g_strndup(str + start, len - start);
}