#ifndef GATE_H
#define GATE_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct gate gate_t;

/**
 * Gate kinds
 */
typedef enum gate_kind_t {
    NAND,
    AND,
    OR,
    NOR,
    XOR,
    XNOR,
} gate_kind_t;

/**
 * @brief Creates a new gate of the specified type with `n` inputs.
 *
 * @param kind The type of gate to create (e.g. `NAND`, `AND`, `OR`).
 * @param n The number of inputs for the gate.
 * @return
 * - Pointer to the created gate structure on success.
 * - `NULL` if memory allocation fails (`errno` is set to `ENOMEM`).
 */
gate_t *gate_new(gate_kind_t kind, unsigned n);

/**
 * @brief Deletes the specified gate.
 *
 * Disconnects all input and output signals from the gate and frees its allocated memory.
 * Does nothing if `g` is `NULL`. After deletion, the pointer `g` becomes invalid.
 *
 * @param g Pointer to the gate to delete.
 */
void gate_delete(gate_t *g);

/**
 * @brief Connects the output of one gate to the input of another gate.
 *
 * Connects the output of `g_out` to the `k`-th input of `g_in`. Any signal previously connected to
 * the `k`-th input of `g_in` will be disconnected.
 *
 * @param g_out Pointer to the output gate.
 * @param g_in Pointer to the input gate.
 * @param k Index of the input in `g_in` to connect.
 * @return
 * - 0 on success.
 * - -1 if any pointer is `NULL`, `k` is invalid, or memory allocation fails (`errno` is set to `EINVAL` or `ENOMEM`).
 */
int gate_connect_gate(gate_t *g_out, gate_t *g_in, unsigned k);

/**
 * @brief Connects a boolean signal to the input of a gate.
 *
 * Connects a boolean signal `s` to the `k`-th input of gate `g`.
 * Any signal previously connected to the `k`-th input will be disconnected.
 *
 * @param s Pointer to the constant boolean signal.
 * @param g Pointer to the gate.
 * @param k Index of the input in `g` to connect.
 * @return
 * - 0 on success.
 * - -1 if any pointer is `NULL`, `k` is invalid, or memory allocation fails (`errno` is set to `EINVAL` or `ENOMEM`).
 */
int gate_connect_signal(bool const *s, gate_t *g, unsigned k);

/**
 * @brief Evaluates the output signals of the specified gates and calculates the critical path length.
 *
 * Computes the output signals of the gates in `g` and stores them in the `s` array.
 * Also calculates the maximum critical path length for the given gates.
 *
 * @param g Array of pointers to gates.
 * @param s Array to store the output signals of the gates.
 * @param m Size of the `g` and `s` arrays.
 * @return
 * - Critical path length on success (also populates the `s` array). s[i] contains the value of the signal at the output of the gate pointed to by g[i].
 * - -1 if any pointer is `NULL`, `m` is zero, operation failed or memory allocation fails (`errno` is set to `EINVAL`, `ECANCELED`, or `ENOMEM`).
 */
ssize_t gate_evaluate(gate_t **g, bool *s, size_t m);

/**
 * @brief Returns the fan-out of the specified gate.
 *
 * Calculates the number of inputs in other gates connected to the output of the given gate.
 *
 * @param g Pointer to the gate.
 * @return
 * - The number of connected inputs on success.
 * - -1 if `g` is `NULL` (`errno` is set to `EINVAL`).
 */
ssize_t gate_fan_out(gate_t const *g);

/**
 * @brief Returns the fan-in of the specified gate.
 *
 * @param g Pointer to the gate.
 * @return
 * - The number of inputs of the gate.
 * - -1 if `g` is `NULL` (`errno` is set to `EINVAL`).
 */
ssize_t gate_fan_in(gate_t const *g);

/**
 * @brief Retrieves the signal or gate connected to a specific input.
 *
 * Returns a pointer to the boolean signal or gate connected to the `k`-th input of the gate `g`.
 * Returns `NULL` if nothing is connected to the input.
 *
 * @param g Pointer to the gate.
 * @param k Index of the input.
 * @return
 * - Pointer to a `bool*` or `gate_t*` on success.
 * - `NULL` if no connection exists (errno is set to 0)
 * = `NULL` if `g` is `NULL`, or `k` is invalid (`errno` is set to `EINVAL`).
 */
void *gate_input(gate_t const *g, unsigned k);

/**
 * @brief Retrieves a gate connected to the output of the specified gate.
 *
 * @param g Pointer to the gate.
 * @param k Index of the connection to retrieve (from 0 to `gate_fan_out(g) - 1`).
 * @return
 * - Pointer to the connected gate.
 * - `NULL` if parameters are invalid or the output is not connected to any gate.
 */
gate_t *gate_output(gate_t const *g, ssize_t k);

#endif
