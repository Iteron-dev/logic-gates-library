#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

#define max(a, b) (a > b ? a : b)

typedef enum input_kind_t in_connection_type_t;
enum input_kind_t {
    SIGNAL,
    GATE
};

typedef struct el_out element_out;
struct el_out {
    void *pointer;
    size_t idx; // The index at which this element is located in the vector.
};
typedef struct vec_out vector_out;
struct vec_out {
    element_out **data;
    size_t size;
    size_t capacity;
};

typedef struct el_in element_in;
struct el_in {
    in_connection_type_t connection_type;
    void *pointer; //  Under this pointer, there may be either a gate or a signal.
    element_out *origin; // A pointer to an element located in the pointer->out array (when the element is a gate).
};
typedef struct vec_in vector_in;
struct vec_in {
    element_in **data;
    size_t size;
    size_t capacity;
};

vector_in *vector_in_init(size_t n);

vector_out *vector_out_init(size_t n);

void vector_in_free(vector_in *vec);

void vector_out_free(vector_out *vec);

size_t vector_in_size(vector_in *vec);

size_t vector_out_size(vector_out *vec);

size_t vector_in_capacity(vector_in *vec);

int vector_out_realloc(vector_out *vec);

void vector_out_push_back(vector_out *vec, element_out *value);

element_in *get_element_in_at_index(vector_in *vec, unsigned index);

element_out *get_element_out_at_index(vector_out *vec, unsigned index);

int vector_in_update_at_index(vector_in *vec, element_in *value, unsigned index);

#endif