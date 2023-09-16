#ifndef JSON_H
#define JSON_H

#include <stdbool.h>
#include <stdint.h>

#include "string.h"

typedef enum JSONType {
    JSON_TYPE_NULL,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_LIST,
    JSON_TYPE_OBJECT,
} JSONType;

typedef struct JSONList {
    JSON ** elements;
    uint32_t length;
} JSONList;

typedef struct JSONKeyPair {
    String key;
    JSONObject * object;
} JSONKeyPair;

typedef struct JSONDictionary {
    JSONKeyPair * key_pairs;
    uint32_t length;
} JSONObject;

typedef struct JSONObject {
    union {
        bool boolean;
        double number;
        String string;
        JSONList list;
        JSONDictionary dictionary;
    };
    JSONType type;
} JSONObject;

typedef struct JSONDocument {
    JSONObject * root;
} JSONDocument;

uint32_t json_document_size(const String data);
void json_document_parse(JSONDocument * document, JSONObject * buffer, const String data);
JSONObject * json_dictionary_lookup(const JSONDictionary dictionary, const String key);


#endif
