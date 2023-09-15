#ifndef STRING_H
#define STRING_H

#include <stdint.h>

typedef struct String {
    const char * string;
    int32_t length;
} String;

#endif
