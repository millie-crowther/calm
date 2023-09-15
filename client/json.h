#ifndef JSON_H
#define JSON_H

#include <stdbool.h>
#include <stdint.h>

typedef enum JSONType {
    JSON_TYPE_NULL,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_LIST,
    JSON_TYPE_OBJECT,
} JSONType;

typedef struct JSON {
    char * start;
} JSON;

JSONType json_infer_type(const JSON * json);
bool json_get_bool(const JSON * json);
int64_t json_get_integer(const JSON * json);


#endif
