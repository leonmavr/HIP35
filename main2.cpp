#include <stdio.h>
#include <stdlib.h>

static void foo() {
    printf("foo from TIOVX\n");
}

/*
 * Table that can achieve the following mappings:
 * state number -> (condition, code)
 * Or:
 * (state number, condition) -> code
 * Or:
 * condition -> code
 */
#define STATE_TABLE(X) \
    X(STATE_0, i == STATE_0, printf("state 0 TIOVX code\n");, printf("state 0 general code\n"); ) \
    X(STATE_1, i == STATE_1, printf("state 1 TIOVX code\n"); \
                             foo();,                          printf("state 1 general code\n"); )

enum {
#define X(a, b, c, d) a,
    STATE_TABLE(X)
#undef X
    NUM_STATES
};


int main() {
    int i = STATE_1;
    // usage 1: in the hardware
#define X(a, b, c, d) if (b) { c };
    STATE_TABLE(X)
#undef X

    // usage 2: in a general-purpose PC (camera repo)
#define X(a, b, c, d) if (b) { printf(#d); };
    STATE_TABLE(X)
#undef X
    return 0;
}
