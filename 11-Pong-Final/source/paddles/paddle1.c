#include <avr/interrupt.h>
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include <stdbool.h>

#include "../header/analog_to_digital.h"
#include "../header/scheduler.h"
#include "../header/timer.h"
#include "../header/utility.h"

// =============================================================================
// ----------------------------- SHARED VARIABLES -----------------------------
static unsigned char pd1 = 0xFD;
static unsigned char pd2 = 0xFD;

// =============================================================================
// --------------------------- variables for pd1_tick --------------------------
static paddle left_paddle = {0xFD, 0xFB, 0xF7};  // the left side paddle

enum pd1_states { pd1_init,
                  pd1_up,
                  pd1_down };

int paddle1_tick(int state) {
    unsigned char A0 = (~PINA & 0x01);  // when A0 is pressed, move up
    unsigned char A1 = (~PINA & 0x02);  // when A1 is pressed, move down

    switch (state) {
        case pd1_init:
            if (A0 && !A1) {
                state = pd1_up;
            } else if (!A0 && A1) {
                state = pd1_down;
            }
            break;
        case pd1_up:
            if (A0 && !A1) {
                state = pd1_up;
            } else {
                state = pd1_init;
            }
            break;
        case pd1_down:
            if (!A0 && A1) {
                state = pd1_down;
            } else {
                state = pd1_init;
            }
            break;
        default:
            state = pd1_init;
            break;
    }

    switch (state) {
        case pd1_init:
            break;
        case pd1_up:
            if (left_paddle.top < TOP) {
                // if we're not at the top, move left_paddle up a row
                left_paddle = moveUp(left_paddle);
                pd1 = left_paddle.top;
            }
            break;
        case pd1_down:
            if (left_paddle.bottom > BOTTOM) {
                // if we're not at the bottom, move left_paddle down a row
                left_paddle = moveDown(left_paddle);
                pd1 = left_paddle.top;
            }
            break;
        default:
            break;
    }
    return state;
}
