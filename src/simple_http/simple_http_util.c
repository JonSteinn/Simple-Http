#include "simple_http_util.h"


int32_t* alloc_int(int32_t integer) {
    int32_t* new_int = (int32_t*)malloc(sizeof(int32_t));
    *new_int = integer;
    return new_int;
}
