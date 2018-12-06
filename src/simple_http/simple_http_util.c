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

/**
 * Given a string, we return its length ignoring any trailing
 * occurrances of '/'. Example: 
 *  func("/")     = 0
 *  func(NULL)    = 0
 *  func("/a/b/") = 4
 */
size_t string_length_without_trailing_forward_slash(const char* string) {
    if (!string) {
        return 0;
    }
    size_t len = strlen(string);
    while (len > 0 && string[len - 1] == '/') {
        len--;
    }
    return len;
}

/**
 * Same as string_length_without_trailing_forward_slash, but
 * caller responsible for knowing and passing down the string's
 * length.
 */
size_t string_length_without_trailing_forward_slash_given_length(const char* string, size_t len) {
    if (!string) {
        return 0;
    }
    while (len > 0 && string[len - 1] == '/') {
        len--;
    }
    return len;
}

/**
 * How many forward slashes in a row are at the start of the string?
 */
size_t string_offset_jumping_leading_forward_slash(const char* string, size_t len) {
    size_t offset = 0;
    while (offset < len && string[offset] == '/') {
        offset++;
    }
   return offset;
}

/**
 * Returns true iff str contains '{' or '}'.
 */
bool contains_curly_brackets(const char* str) {
    if (!str) return false;
    for (int32_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '{' || str[i] == '}') {
            return true;
        }
    }
    return false;
}