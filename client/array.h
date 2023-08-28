#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>

#define MINIMUM_ARRAY_CAPACITY 10
#define ARRAY_RESIZE_FACTOR 2

#define Array(T) struct {  \
    uint32_t size;         \
    uint32_t capacity;     \
    uint32_t element_size; \
    T * data;              \
}

#define array_create(array) do {                              \
    uint32_t element_size = sizeof(*((array)->data));         \
    *array = (typeof(*(array))) {                             \
        .size = 0,                                            \
        .capacity = MINIMUM_ARRAY_CAPACITY,                   \
        .element_size = element_size,                         \
        .data = malloc(element_size * MINIMUM_ARRAY_CAPACITY) \
    };                                                        \
} while (0)

#define array_destroy(array) do {     \
    typeof(array) array_copy = array; \
    free(array_copy->data);           \
    *array_copy = (typeof(*array)){   \
        .size = 0,                    \
        .capacity = 0,                \
        .element_size = 0,            \
        .data = NULL,                 \
    };                                \
} while(0)

#define array_is_empty(array) ((array)->size == 0)

#define array_push_back(array, element) do {                           \
    typeof(array) array_copy = array;                                  \
    array_copy->size++;                                                \
    if (array_copy->size > array_copy->capacity){                      \
        array_copy->capacity = array_copy->size * ARRAY_RESIZE_FACTOR; \
        array_copy->data = realloc(                                    \
            array_copy->data,                                          \
            array_copy->capacity * array_copy->element_size            \
        );                                                             \
    }                                                                  \
    array_copy->data[array_copy->size - 1] = element;                  \
} while (0)

#define array_pop_back(array, element) do {                                  \
    typeof(array) array_copy = array;                                        \
    typeof(element) element_copy = element;                                  \
    if (!array_is_empty(array_copy)){                                        \
        if (element_copy != NULL){                                           \
            *(element_copy) = array_copy->data[array_copy->size - 1];        \
        }                                                                    \
        array_copy->size--;                                                  \
        if (                                                                 \
            array_copy->size < array_copy->capacity / ARRAY_RESIZE_FACTOR && \
            array_copy->size >= MINIMUM_ARRAY_CAPACITY                       \
        ){                                                                   \
            array_copy->capacity = array_copy->size;                         \
            array_copy->data = realloc(                                      \
                array_copy->data,                                            \
                array_copy->capacity * array_copy->element_size              \
            );                                                               \
        }                                                                    \
    }                                                                        \
} while (0)

#endif