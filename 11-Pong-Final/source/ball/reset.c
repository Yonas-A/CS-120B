#include <stdbool.h>

#include "../header/utility.h"

// =============================================================================
// -------------------------- variables for reset_tick --------------------------

enum reset_states { reset_init,
                    reset_start };

int reset_tick(int state) {
    unsigned char A5 = (~PINA & 0x10);  // when A5 is pressed, reset score and matrix
    switch (state) {
        case reset_init:
            if (A5) {
                state = reset_start;
            }
            break;
        case reset_start:
            if (!A5) {
                state = reset_init;
            }
            break;
        default:
            state = reset_init;
            break;
    }
    switch (state) {
        case reset_init:
            break;
        case reset_start:
            reset();
            break;
        default:
            break;
    }
    return state;
}