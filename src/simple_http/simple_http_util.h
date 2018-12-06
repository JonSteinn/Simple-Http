/*************************************************************
 * Written by: Jón Steinn Elíasson                           *
 *                                                           *
 * Contains various helper functions.                        *
 *************************************************************/

#ifndef eE1GrXMPEu_SIMPLE_HTTP_UTIL_H
#define eE1GrXMPEu_SIMPLE_HTTP_UTIL_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <glib.h>

int32_t* alloc_int(int32_t integer);
char* trim_end_white_spaces_alloc(const char* str);
size_t string_length_without_trailing_forward_slash(const char* string);
size_t string_length_without_trailing_forward_slash_given_length(const char* string, size_t len);
size_t string_offset_jumping_leading_forward_slash(const char* string, size_t len);
bool contains_curly_brackets(const char* str);

#endif
