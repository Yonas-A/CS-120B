#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/analog_to_digital.h"
#include "../header/scheduler.h"
#include "../header/timer.h"
#include "../header/utility.h"

// =============================================================================
// ----------------------------- SHARED VARIABLES -----------------------------
static unsigned char pd1 = 0xFD;
static unsigned char pd2 = 0xFD;

// =============================================================================
// --------------------------- variables for pd2_tick --------------------------
static paddle right_paddle = {0xFD, 0xFB, 0xF7};

enum pd2_states { pd2_init,
                  pd2_up,
                  pd2_down };

int paddle2_tick(int state) {
    unsigned short joystick_input = ADC;

    switch (state) {
        case pd2_init:
            if (joystick_input > 750) {
                state = pd2_up;
            } else if (joystick_input < 450) {
                state = pd2_down;
            }
            break;
        case pd2_up:
            if (joystick_input > 750) {
                state = pd2_up;
            } else {
                state = pd2_init;
            }
            break;
        case pd2_down:
            if (joystick_input < 450) {
                state = pd2_down;
            } else {
                state = pd2_init;
            }
            break;
        default:
            state = pd2_init;
            break;
    }

    switch (state) {
        case pd2_init:
            break;
        case pd2_up:
            if (right_paddle.top < TOP) {
                // if we're not at the top, move right_paddle up a row
                right_paddle = moveUp(right_paddle);
                pd2 = right_paddle.top;
            }
            break;
        case pd2_down:
            if (right_paddle.bottom > BOTTOM) {
                // if we're not at the bottom, move right_paddle down a row
                right_paddle = moveDown(right_paddle);
                pd2 = right_paddle.top;
            }
            break;
        default:
            break;
    }
    return state;
}