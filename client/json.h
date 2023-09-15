#ifndef JSON_H
#define JSON_H

#include <stdbool.h>
#include <stdint.h>

typedef struct JSON {
    union JSONData {
        bool boolean;
        int64_t integer;
        double number;
        
    };
    char * start;
} JSON;

#endif
