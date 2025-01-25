#include <stddef.h>
#include <stdlib.h>

#include "vector.h"

vector_in *vector_in_init(size_t n) {
    vector_in *vec = (vector_in *) malloc(sizeof(vector_in));
    if (vec == NULL) {
        return NULL;
    }

    element_in **data = (element_in **) calloc(n, sizeof(element_in *));
    if (data == NULL) {
        free(vec);
        return NULL;
    }

    vec->data = data;
    vec->size = 0;
    vec->capacity = n;

    return vec;
}

vector_out *vector_out_init(size_t n) {
    vector_out *vec = (vector_out *) malloc(sizeof(vector_out));
    if (vec == NULL) {
        return NULL;
    }

    element_out **data = (element_out **) calloc(n, sizeof(element_out *));
    if (data == NULL) {
        free(vec);
        return NULL;
    }

    vec->data = data;
    vec->size = 0;
    vec->capacity = n;

    return vec;
}

void vector_in_free(vector_in *vec) {
    vec->size = vec->capacity = 0;

    free(vec->data);
    vec->data = NULL;
    free(vec);
    vec = NULL;
}

void vector_out_free(vector_out *vec) {
    vec->size = vec->capacity = 0;

    free(vec->data);
    vec->data = NULL;
    free(vec);
    vec = NULL;
}

size_t vector_in_size(vector_in *vec) {
    return vec->size;
}

size_t vector_out_size(vector_out *vec) {
    return vec->size;
}

size_t vector_in_capacity(vector_in *vec) {
    return vec->capacity;
}

int vector_out_realloc(vector_out *vec) {
    if (vec->size == 0) {
        return 0;
    }

    size_t new_capacity = vec->capacity;
    if (vec->capacity == vec->size) {
        new_capacity *= 2;
    } else if (vec->capacity >= 4 * vec->size) {
        new_capacity /= 2;
    } else {
        return 0;
    }

    element_out **data = (element_out **) realloc(vec->data, new_capacity * sizeof(element_out *));
    if (data == NULL) {
        return -1;
    }
    vec->data = data;
    vec->capacity = new_capacity;
    return 0;
}

void vector_out_push_back(vector_out *vec, element_out *value) {
    vec->data[vec->size] = value;
    vec->size++;
}

element_in *get_element_in_at_index(vector_in *vec, unsigned index) {
    return vec->data[index];
}

element_out *get_element_out_at_index(vector_out *vec, unsigned index) {
    return vec->data[index];
}

int vector_in_update_at_index(vector_in *vec, element_in *value, unsigned index) {
    if (get_element_in_at_index(vec, index) == NULL) {
        vec->size++;
    }

    vec->data[index] = value;
    return 0;
}

int vector_out_update_at_index(vector_out *vec, element_out *value, unsigned index) {
    if (get_element_out_at_index(vec, index) == NULL) {
        vec->size++;
    }

    vec->data[index] = value;
    return 0;
}