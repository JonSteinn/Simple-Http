#include "simple_http_util.h"


int32_t* alloc_int(int32_t integer) {
    int32_t* new_int = (int32_t*)malloc(sizeof(int32_t));
    *new_int = integer;
    return new_int;
}

char* trim_end_white_spaces_alloc(char* str) {
    size_t len = strlen(str);

    while (len > 0 && str[len - 1] == ' ') {
        len--;
    }

    size_t start = 0;
    while (start < len && str[start] == ' ') {
        start++;
    }

    return len == 0 ? NULL : g_strndup(str + start, len - start);
}