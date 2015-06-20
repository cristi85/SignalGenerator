#ifndef _STRING_H
#define _STRING_H

#include "types.h"

char* string_U32ToStr(u32, char*);
char* string_copy(char*, const char*);
char* string_append(char*, const char*);
char* string_substr(char*, const char*, u8, u8);
u8 string_len(const char*);

#endif