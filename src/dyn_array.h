#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <memory.h>

/// A struct that can be used to create an array that automatically resizes it's contents
typedef struct DynArray {
    /// the pointer to the location in memory where the array is located
    void* elements;

    /// Amount of elements currently in the array
    size_t length;

    /// The total capacity of the array, this value should generally not be accessed outside of this implementation
    size_t capacity;

    /// The size of each element in bytes
    uint16_t stride;
} DynArray;

inline DynArray dynArrayInit(size_t starting_capacity, uint16_t elem_size) {
    if (starting_capacity == 0) {
        return (DynArray){
            .elements = nullptr,
            .length = 0,
            .capacity = 0,
            .stride = elem_size,
        };
    }

    void* data = malloc(starting_capacity * elem_size);
    if (!data) {
        printf("Out Of Memory");
        exit(EXIT_FAILURE);
    }

    return (DynArray){
        .elements = data,
        .length = 0,
        .capacity = starting_capacity,
        .stride = elem_size,
    };
}

inline void dynArrayAppend(DynArray* arr, const void* item) {
    size_t new_len = arr->length + 1;

    if (new_len > arr->capacity) {
        size_t new_capacity = arr->capacity < 8 ? 8 : arr->capacity * 2;
        size_t num_bytes = new_capacity * arr->stride;
        arr->elements = realloc(arr->elements, num_bytes);
        arr->capacity = new_capacity;
    }

    void* new_item_pos = (uint8_t*)arr->elements + arr->length * arr->stride;
    memcpy(new_item_pos, item, arr->stride);

    arr->length = new_len;
}

inline void* dynArrayGet(DynArray* arr, size_t pos) {
    if (pos >= arr->length) {
        return nullptr;
    }

    return (uint8_t*)arr->elements + pos * arr->stride;
}



#define CREATE_DYNAMIC_ARRAY(T, name)                                           \
typedef struct DynArray##name {                                                 \
    /** the pointer to the location in memory where the array is located */     \
    T* elements;                                                                \
                                                                                \
    /** Amount of elements currently in the array */                            \
    size_t length;                                                              \
                                                                                \
    /** The total capacity of the array,                                        
      * this value should generally not be accessed outside of this implementation
      */                                                                        \
    size_t capacity;                                                            \
} DynArray##name;                                                               \
                                                                                \
DynArray##name dynArray##name##Init(size_t starting_capacity) {                 \
    if (starting_capacity == 0) {                                               \
        return (DynArray##name){                                                \
            .elements = nullptr,                                                \
            .length = 0,                                                        \
            .capacity = 0,                                                      \
        };                                                                      \
    }                                                                           \
                                                                                \
    void* data = malloc(starting_capacity * sizeof(T));                         \
    if (!data) {                                                                \
        printf("Out Of Memory");                                                \
        exit(EXIT_FAILURE);                                                     \
    }                                                                           \
                                                                                \
    return (DynArray##name){                                                    \
        .elements = data,                                                       \
        .length = 0,                                                            \
        .capacity = starting_capacity,                                          \
    };                                                                          \
}                                                                               \
                                                                                \
                                                                                \
void dynArray##name##Append(DynArray##name* arr, const T item) {                \
    size_t new_len = arr->length + 1;                                           \
                                                                                \
    if (new_len > arr->capacity) {                                              \
        size_t new_capacity = arr->capacity < 8 ? 8 : arr->capacity * 2;        \
        size_t num_bytes = new_capacity * sizeof(T);                            \
        arr->elements = (T*)realloc(arr->elements, num_bytes);                  \
        arr->capacity = new_capacity;                                           \
    }                                                                           \
                                                                                \
    T* new_item_pos = arr->elements + arr->length;                              \
    memcpy(new_item_pos, &item, sizeof(T));                                     \
}
