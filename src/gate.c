#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

#include "gate.h"
#include "vector.h"

// Enum for NAND gate states
typedef enum state {
    UNVISITED, // Unvisited gate
    VISITED,   // Visited gate, but not fully calculated
    CALCULATED // Visited and fully calculated gate
} state;

typedef enum error_code {
    FAILED = -1,
    SUCCESS = 0,
} error_code;

// Auxiliary structure for passing an error code
typedef struct res_with_code {
    bool res;
    int code;
} res_with_code;

struct gate {
    vector_in *in;
    vector_out *out;
    state state;
    bool res;
    unsigned path_len;
    gate_kind_t kind;
};

static int vector_out_delete_element(vector_out *vec, element_out *el) {
    size_t last_idx = vector_out_size(vec) - 1;

    el->pointer = vec->data[last_idx]->pointer;
    el->idx = vec->data[last_idx]->idx;

    const gate_t *g_under = el->pointer;
    element_in *g_under_el = get_element_in_at_index(g_under->in, el->idx);
    g_under_el->origin = el;

    // Releasing the memory of the last element of the vector
    free(vec->data[last_idx]);
    vec->data[last_idx] = NULL;

    vec->size--;

    return 0;
}

gate_t *gate_new(gate_kind_t kind, unsigned n) {
    gate_t *g = malloc(sizeof(gate_t));
    if (g == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    vector_in *in = vector_in_init(n);
    if (in == NULL) {
        free(g);
        errno = ENOMEM;
        return NULL;
    }

    vector_out *out = vector_out_init(1);
    if (out == NULL) {
        free(g);
        vector_in_free(in);
        errno = ENOMEM;
        return NULL;
    }

    g->in = in;
    g->out = out;
    g->state = UNVISITED;
    g->path_len = 1;
    g->kind = kind;

    return g;
}

int gate_connect_gate(gate_t *g_out, gate_t *g_in, unsigned k) {
    if (g_out == NULL || g_in == NULL || k >= vector_in_capacity(g_in->in)) {
        errno = EINVAL;
        return FAILED;
    }

    element_in *el_in = malloc(sizeof(element_in));
    if (el_in == NULL) {
        errno = ENOMEM;
        return FAILED;
    }

    element_out *el_out = malloc(sizeof(element_out));
    if (el_out == NULL) {
        free(el_in);
        errno = ENOMEM;
        return FAILED;
    }

    // el_in represents the element (which is a gate) that is being connected to the input array of the g_in gate.
    el_in->connection_type = GATE;
    el_in->pointer = (void *) g_out;
    el_in->origin = el_out; // A pointer to the element el_out, which points to g_in.

    // `el_out` represents the element (which is a gate) that is being connected to the output array of the `g_out` gate.
    el_out->pointer = (void *) g_in;
    el_out->idx = k; // The index in the in vector of the g_in gate where the element pointing to the g_out gate is located.

    if (vector_out_realloc(g_out->out) != 0) {
        free(el_in);
        free(el_out);
        errno = ENOMEM;
        return FAILED;
    }

    element_in *in_value = get_element_in_at_index(g_in->in, k);
    if (in_value != NULL && in_value->connection_type == GATE) {
        gate_t *g_out_old = in_value->pointer;
        if (g_out_old != NULL && g_out_old->out != NULL) {
            vector_out_delete_element(g_out_old->out, in_value->origin);
        }
    }
    vector_out_push_back(g_out->out, el_out);

    vector_in_update_at_index(g_in->in, el_in, k);
    free(in_value);

    return SUCCESS;
}

int gate_connect_signal(bool const *s, gate_t *g, unsigned k) {
    if (s == NULL || g == NULL || k >= vector_in_capacity(g->in)) {
        errno = EINVAL;
        return FAILED;
    }

    element_in *el = malloc(sizeof(element_in));
    if (el == NULL) {
        errno = ENOMEM;
        return FAILED;
    }

    el->connection_type = SIGNAL;
    el->pointer = (void *) s;

    element_in *in_value = get_element_in_at_index(g->in, k);
    if (in_value != NULL && in_value->connection_type == GATE) {
        gate_t *g_out_old = (gate_t *) in_value->pointer;
        if (g_out_old != NULL && g_out_old->out != NULL) {
            vector_out_delete_element(g_out_old->out, in_value->origin);
        }
    }

    vector_in_update_at_index(g->in, el, k);
    free(in_value);
    return SUCCESS;
}

void gate_delete(gate_t *g) {
    if (g == NULL) {
        return;
    }

    vector_in *in = g->in;

    // Removing elements that enter the gate
    for (size_t i = 0; i < vector_in_capacity(in); ++i) {
        element_in *in_value = get_element_in_at_index(in, i);

        if (in_value != NULL && in_value->connection_type == GATE) {
            gate_t *g_old = (gate_t *) in_value->pointer;
            if (g_old != NULL && g_old->out != NULL) {
                vector_out_delete_element(g_old->out, in_value->origin);
            }
        }
        free(in_value);
        in_value = NULL;
    }

    vector_in_free(in);

    vector_out *out = g->out;

    // Removing elements that exit the gate
    for (size_t i = 0; i < out->size; ++i) {
        element_out *out_value = get_element_out_at_index(out, i);
        if (out_value != NULL) {
            gate_t *g_old = (gate_t *) out_value->pointer;
            if (g_old != NULL && g_old->in != NULL && g_old->in->data != NULL) {
                free(g_old->in->data[out_value->idx]);
                g_old->in->data[out_value->idx] = NULL;
                g_old->in->size--;
            }
        }

        free(out_value);
        out_value = NULL;
    }

    vector_out_free(out);

    free(g);
}

// A function that recursively restores the state of the gate to its state before the nand_evaluate function was called
static void nand_clean_recursive(gate_t *g) {
    if (g == NULL) return;

    if (g->state == UNVISITED) return;

    g->state = UNVISITED;
    for (size_t i = 0; i < vector_in_size(g->in); ++i) {
        element_in *in_value = get_element_in_at_index(g->in, i);
        if (in_value != NULL && in_value->connection_type == GATE) {
            gate_t *g_in = (gate_t *) in_value->pointer;
            if (g_in->state != UNVISITED) nand_clean_recursive(g_in);
        }
    }
}

// A function that clears all gates from the gate array g
static void nand_clean(gate_t **g, size_t m) {
    for (size_t i = 0; i < m; ++i) {
        if (g[i] != NULL) {
            nand_clean_recursive(g[i]);
        }
    }
}

static int calculate_result(bool *res_value, bool signal, gate_kind_t kind) {
    switch (kind) {
        case AND:
        case NAND:
            *res_value &= signal;
            break;
        case OR:
        case NOR:
            *res_value |= signal;
            break;
        case XOR:
        case XNOR:
            *res_value *= signal;
            break;
        default:
            return -1;
    }

    return 0;
}

// A function that recursively traverses the gates connected to gate g, calculating the boolean signal and
// the maximum critical path for the given gate
static res_with_code nand_evaluate_recursive(gate_t *g) {
    if (g == NULL || vector_in_size(g->in) != vector_in_capacity(g->in)) {
        return (res_with_code){.res = false, .code = FAILED};
    }

    if (g->state == CALCULATED) {
        return (res_with_code){.res = g->res, .code = SUCCESS};
    }

    if (g->state == VISITED) {
        // We have found a cycle
        return (res_with_code){.res = false, .code = FAILED};
    }

    g->state = VISITED;

    g->path_len = 0;

    bool res_value = true;
    if (g->kind == OR || g->kind == NOR || g->kind == XNOR || g->kind == XOR) {
        res_value = false; // Only for OR and NOR gates, the initial result must be false.
    }

    for (size_t i = 0; i < vector_in_size(g->in); ++i) {
        element_in *in_value = get_element_in_at_index(g->in, i);
        if (in_value == NULL) {
            continue;
        }

        if (in_value->connection_type == SIGNAL) {
            bool const *signal = in_value->pointer;
            const int status_code = calculate_result(&res_value, *signal, g->kind);
            if (status_code == -1) {
                return (res_with_code){false, FAILED};
            }
        } else {
            gate_t *g_in = in_value->pointer;
            if (g == g_in) {
                return (res_with_code){false, FAILED};
            }

            res_with_code res = nand_evaluate_recursive(g_in);
            if (res.code != SUCCESS) {
                return res;
            }

            const int status_code = calculate_result(&res_value, res.res, g->kind);
            if (status_code == -1) {
                return (res_with_code){false, FAILED};
            }

            g->path_len = max(g->path_len, g_in->path_len);
        }
    }

    if (vector_in_size(g->in) > 0) {
        g->path_len++;
    }

    g->state = CALCULATED;

    if (g->kind == NAND || g->kind == NOR || g->kind == XNOR) {
        g->res = !res_value; //  Calculating the gate's output signal. Negation because we are computing "N" gates.
    } else {
        g->res = res_value;
    }

    return (res_with_code){.res = g->res, .code = SUCCESS};
}

ssize_t gate_evaluate(gate_t **g, bool *s, size_t m) {
    if (g == NULL || s == NULL || m == 0) {
        errno = EINVAL;
        return FAILED;
    }

    for (size_t i = 0; i < m; ++i) {
        if (g[i] == NULL) {
            errno = EINVAL;
            return FAILED;
        }
    }

    ssize_t res = 0;


    for (size_t i = 0; i < m; ++i) {
        if (vector_in_size(g[i]->in) != vector_in_capacity(g[i]->in)) {
            nand_clean(g, m);
            errno = ECANCELED;
            return FAILED;
        }

        res_with_code res_code = nand_evaluate_recursive(g[i]);
        if (res_code.code != 0) {
            nand_clean(g, m);
            errno = ECANCELED;
            return FAILED;
        }

        s[i] = res_code.res;
        res = max(g[i]->path_len, res);
    }

    nand_clean(g, m);

    return res;
}

ssize_t gate_fan_out(gate_t const *g) {
    if (g == NULL) {
        errno = EINVAL;
        return FAILED;
    }

    return (ssize_t) vector_out_size(g->out);
}

ssize_t gate_fan_in(gate_t const *g) {
    if (g == NULL) {
        errno = EINVAL;
        return FAILED;
    }

    return (ssize_t) vector_in_size(g->in);
}

void *gate_input(gate_t const *g, unsigned k) {
    if (g == NULL || k >= vector_in_capacity(g->in)) {
        errno = EINVAL;
        return NULL;
    }

    element_in const *el = get_element_in_at_index(g->in, k);
    if (el == NULL) {
        errno = 0;
        return NULL;
    }

    return el->pointer;
}

gate_t *gate_output(gate_t const *g, ssize_t k) {
    if (g == NULL || k < 0 || k >= gate_fan_out(g)) {
        return NULL;
    }

    element_out *el = get_element_out_at_index(g->out, k);
    if (el == NULL) {
        return NULL;
    }

    return (gate_t *) el->pointer;
}
