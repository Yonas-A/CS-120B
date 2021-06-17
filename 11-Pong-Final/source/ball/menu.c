#include <avr/interrupt.h>
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../latest5za.c"

// =============================================================================
// --------------------------- variables for menu_tick --------------------------

/**
 * Resets every variable to a default value
 */
void reset() {
    pd1 = pd2 = 0xFD;
    ballSpeed = 0x64;  // 100
    curr = updatePosition(curr, 0xFB, 0x08);
    prev = updatePosition(prev, 0xFB, 0x10);
    left_paddle = updatePaddle(left_paddle, 0xFD, 0xFB, 0xF7);
    right_paddle = updatePaddle(right_paddle, 0xFD, 0xFB, 0xF7);
    is2Player = false;
    startGame = endGame = false;
    player1Win = player2Win = false;
}

enum menu_states { menu_init,
                   menu_select1,
                   menu_wait1,
                   menu_select2,
                   menu_wait2 };

int Tick_menu(int state) {
    unsigned char A6 = (~PINA & 0x40);  // when A6 is pressed, initiate menu setup

    switch (state) {
        case menu_init:
            // if (A6 && !startGame) {
            //     state = menu_select1;
            // }
            if (A6) {
                state = menu_select1;
            }
            break;
        case menu_select1:
            if (!A6) {
                state = menu_wait1;
            }
            break;
        case menu_wait1:
            // if (A6 && !startGame) {
            //     state = menu_select2;
            // }
            if (A6) {
                state = menu_select2;
            }
            break;
        case menu_select2:
            if (!A6) {
                state = menu_wait2;
            }
            break;
        case menu_wait2:
            // if (!A6 && !startGame) {
            //     state = menu_init;
            // }
            if (!startGame) {
                state = menu_init;
            }            
            break;
        default:
            state = menu_init;
            break;
    }
    switch (state) {
        case menu_init:
            break;
        case menu_select1:
            reset();
            is2Player = false;
            startGame = true;
            player1Score = player2Score = 0x00;
            break;
        case menu_wait1:
            break;
        case menu_select2:
            reset();
            is2Player = true;
            startGame = true;
            player1Score = player2Score = 0x00;
            break;
        case menu_wait2:
            break;
        default:
            break;
    }
    return state;
}
