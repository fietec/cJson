#ifndef _STRLIB_H
#define _STRLIB_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "types.h"

#define BYTE_DEL 1024.0

const char* BYTE_SFXS[] = {"B", "KB", "MB", "GB", "TB"};

bool str_is_int(char* s);
bool str_is_float(char* s);

char* str_to_higher(char* s);
char* str_to_lower(char* s);

size_t str_size(const char* s);

char* str_dup(char* s);

int str_char_index(char* s, char c);
void str_char_remove(char* s, char c);
void str_char_replace(char*s, char o, char n);
char* str_human_bytes(char* buffer, size_t buff_len, f_size bytes);

#include "src/strlib.c"

#endif // _STRLIB_H