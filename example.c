#ifdef NDEBUG
#undef NDEBUG
#endif

#include "src/gate.h"
#include <assert.h>
#include <stdbool.h>

static int example(void) {
    gate_t *g0 = gate_new(AND, 2);
    gate_t *g1 = gate_new(NAND, 2);
    gate_t *g2 = gate_new(NAND, 2);

    bool b0 = 0;
    bool b1 = 1;
    bool b = 1;

    gate_connect_gate(g1, g0, 0);
    gate_connect_gate(g2, g0, 1);

    gate_connect_signal(&b0, g1, 0);
    gate_connect_signal(&b0, g1, 1);
    gate_connect_signal(&b0, g2, 0);
    gate_connect_signal(&b1, g2, 1);


    assert(gate_evaluate(&g0, &b, 1) == 2);
    assert(b == 1);

    gate_t *g4 = gate_new(XNOR, 2);
    gate_connect_gate(g1, g4, 0);
    gate_connect_gate(g0, g4, 1);
    assert(gate_evaluate(&g4, &b, 1) == 3);
    assert(b == 1);

    gate_delete(g0);
    gate_delete(g1);
    gate_delete(g2);
    gate_delete(g4);

    return 0;
}

int main(void) {
    return example();
}
