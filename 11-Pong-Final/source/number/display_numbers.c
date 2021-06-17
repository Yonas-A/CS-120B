#include <avr/interrupt.h>
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include <stdbool.h>

#include "../header/constants.h"
#include "../header/scheduler.h"
#include "../header/timer.h"
#include "../header/utility.h"

static unsigned char leftWinCount = 0x00;   // left side player score
static unsigned char rightWinCount = 0x00;  // right side player score

static const bool leftWon = true;
static const bool rightWon = true;

enum winner_state { sm_init,
                    sm_leftWin,
                    sm_rightWin };

int winner_Tick(int state) {
    static pos displayWinner = {0x00, 0x00};
    static unsigned char i = 0x00;

    switch (state) {
        case sm_init:
            if (leftWon == true || rightWon == true) {
                state = sm_leftWin;
            }
            break;
        case sm_leftWin:
            if (leftWinCount == 0 && i < ZERO_LENGTH) {
                state = sm_leftWin;
            } else if (leftWinCount == 1 && i < ONE_LENGTH) {
                state = sm_leftWin;
            } else if (leftWinCount == 2 && i < TWO_LENGTH) {
                state = sm_leftWin;
            } else if (leftWinCount == 3 && i < THREE_LENGTH) {
                state = sm_leftWin;
            } else {
                i = 0;
                state = sm_rightWin;
            }
            break;
        case sm_rightWin:
            if (rightWinCount == 0 && i < ZERO_LENGTH) {
                state = sm_rightWin;
            } else if (rightWinCount == 1 && i < ONE_LENGTH) {
                state = sm_rightWin;
            } else if (rightWinCount == 2 && i < TWO_LENGTH) {
                state = sm_rightWin;
            } else if (rightWinCount == 3 && i < THREE_LENGTH) {
                state = sm_rightWin;
            } else {
                state = sm_init;
                i = 0;
            }
            break;
        default:
            state = sm_init;
            break;
    }
    switch (state) {
        case sm_init:
            break;
        case sm_leftWin:
            if (leftWinCount == 0) {
                displayWinner.row = ZERO_ROW[i];
                displayWinner.pattern = ZERO_PATTERN[i];
                i++;
            } else if (leftWinCount == 1) {
                displayWinner.row = ONE_ROW[i];
                displayWinner.pattern = ONE_PATTERN[i];
                i++;
            } else if (leftWinCount == 2) {
                displayWinner.row = TWO_ROW[i];
                displayWinner.pattern = TWO_PATTERN[i];
                i++;
            } else if (leftWinCount == 3) {
                displayWinner.row = THREE_ROW[i];
                displayWinner.pattern = THREE_PATTERN[i];
                i++;
            }
            break;
        case sm_rightWin:
            if (rightWinCount == 0) {
                displayWinner.row = ZERO_ROW[i];
                displayWinner.pattern = ZERO_PATTERN[i] >> 0x05;
                i++;
            } else if (rightWinCount == 1) {
                displayWinner.row = ONE_ROW[i];
                displayWinner.pattern = ONE_PATTERN[i] >> 0x05;
                i++;
            } else if (rightWinCount == 2) {
                displayWinner.row = TWO_ROW[i];
                displayWinner.pattern = TWO_PATTERN[i] >> 0x05;
                i++;
            } else if (rightWinCount == 3) {
                displayWinner.row = THREE_ROW[i];
                displayWinner.pattern = THREE_PATTERN[i] >> 0x05;
                i++;
            }
            break;
        default:
            break;
    }
    PORTC = displayWinner.pattern;
    PORTD = displayWinner.row;

    return state;
}

enum button_state { b_init,
                    b_up,
                    b_down };

int button_tick(int state) {
    switch (state) {
        case b_init:
            if (~PINA & 0x01) {
                state = b_up;
            } else if (~PINA & 0x02) {
                state = b_down;
            }
            break;
        case b_up:
        case b_down:
            state = b_init;
        default:
            state = b_init;
            break;
    }

    switch (state) {
        case b_init:
            break;
        case b_up:
            if (leftWinCount < 3) {
                leftWinCount++;
            }
            break;
        case b_down:
            if (rightWinCount < 3) {
                rightWinCount++;
            }
            break;
        default:
            break;
    }
    return state;
}

int main(void) {
    DDRA = 0x00;
    PORTA = 0xFF;  // config PA[7:0] as inputs, init to 1's
    DDRC = 0xFF;
    PORTC = 0x00;  // config PC[7:0] as outputs,
    DDRD = 0xFF;
    PORTD = 0x00;  // config PD[7:0] as outputs,

    static task task1, task2;
    task *tasks[] = {&task1, &task2};
    const unsigned short numTasks = sizeof(tasks) / sizeof(task *);

    const char start = -1;

    // Task 1 displayInput
    task1.state = start;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &winner_Tick;

    // adjust numbers
    task2.state = start;
    task2.period = 200;
    task2.elapsedTime = task2.period;
    task2.TickFct = &button_tick;

    unsigned int i = 0x00;
    unsigned long GCD = tasks[0]->period;

    for (i = 1; i < numTasks; i++) {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();

    while (1) {
        for (i = 0; i < numTasks; i++) {
            if (tasks[i]->elapsedTime == tasks[i]->period) {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += GCD;
        }

        while (!TimerFlag)
            ;
        TimerFlag = 0;
    }
    return 0;  // Error: Program should not exit!
}
