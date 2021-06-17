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
static unsigned char pd1 = 0xFD;  // holds the current row of left_paddle being displayed by display_tick

static bool endGame = false;    // holds if the game has ended or not
static bool is2Player = false;  // holds if the game is a 2-player game or not

static paddle left_paddle = {0xFD, 0xFB, 0xF7};  // the left side paddle

// =============================================================================
// ----------------------- variables for ai_paddle_tick -----------------------

enum ai_states { ai_pd_init,
                 ai_pd_move };

int ai_paddle_tick(int state) {
    unsigned int random = next(0, 4);  // holds a random number between 0 and 4
    switch (state) {
        case ai_pd_init:
            if (!is2Player && !endGame) {
                state = ai_pd_move;
            }
            break;
        case ai_pd_move:
            state = ai_pd_init;
            break;
        default:
            state = ai_pd_init;
            break;
    }

    switch (state) {
        case ai_pd_init:
            break;
        case ai_pd_move:
            if (random == 1 && left_paddle.top < TOP) {
                // if we're not at the top, move left_paddle up a row
                left_paddle = moveUp(left_paddle);
                pd1 = left_paddle.top;
            } else if (random == 2 && left_paddle.top < 0xFD) {
                // if we're not at the top, move left_paddle up a row
                left_paddle = moveUp(left_paddle);
                pd1 = left_paddle.top;
            } else if (random == 3 && left_paddle.bottom > BOTTOM) {
                // if we're not at the top, move left_paddle up a row
                left_paddle = moveDown(left_paddle);
                pd1 = left_paddle.top;
            } else if (random == 4 && left_paddle.bottom > 0xF7) {
                // if we're not at the bottom, move left_paddle up a row
                left_paddle = moveDown(left_paddle);
                pd1 = left_paddle.top;
            }
            break;
        default:
            break;
    }
    return state;
}