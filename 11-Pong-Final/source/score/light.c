#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/scheduler.h"
#include "../header/timer.h"

// =============================================================================
// -------------------------- variables for score_tick --------------------------

enum score_states { light_init,
                    light_on,
                    light_wait };

unsigned char player1Score = 0x00;
unsigned char player2Score = 0x00;

int score_tick(int state) {
    static unsigned char output = 0x00;

    switch (state) {
        case light_init:
            state = light_wait;
            break;
        case light_wait:
            if (player1Win) {
                player1Score++;
                state = light_on;
            } else if (player2Win) {
                player2Score++;
                state = light_on;
            }
            break;
        case light_on:
            player2Win = player1Win = false;
            state = light_wait;
            break;
        default:
            state = light_init;
            break;
    }
    switch (state) {
        case light_init:
            player1Score = player2Score = 0x00;
            player1Win = player2Win = false;
            break;
        case light_on:
            if (player1Score == 1)
                output = (output & 0xF8) | 0x01;
            else if (player1Score == 2)
                output = (output & 0xF8) | 0x03;
            else if (player1Score == 3)
                output = (output & 0xF8) | 0x07;

            if (player2Score == 1)
                output = (output & 0x07) | 0x08;
            else if (player2Score == 2)
                output = (output & 0x07) | 0x18;
            else if (player2Score == 3)
                output = (output & 0x07) | 0x38;
            break;
        case light_wait:
            break;
        default:
            state = light_init;
            break;
    }

    PORTB = output;
    return state;
}

enum increase_sm { sm_init,
                   sm_wait,
                   sm_pl1,
                   sm_pl2 };

int increase(int state) {
    unsigned char A0 = (~PINA & 0x01);
    unsigned char A1 = (~PINA & 0x02);
    switch (state) {
        case sm_init:
            state = sm_wait;
            break;
        case sm_wait:
            if (!A0 && !A1) {
                state = sm_wait;
            } else if (A0 && !A1) {
                player1Win = true;
                state = sm_pl1;
            } else if (!A0 && A1) {
                player2Win = true;
                state = sm_pl2;
            }
            break;
        case sm_pl1:
        case sm_pl2:
            if (!A0 && !A1) {
                state = sm_wait;
            }
            break;
        default:
            state = sm_init;
            break;
    }
    switch (state) {
        case sm_init:
            player1Score = player2Score = 0x00;
            player1Win = player2Win = false;
            break;
        case sm_wait:
            break;
        case sm_pl1:
            break;
        case sm_pl2:
            break;
        default:
            break;
    }
}

int main(void) {
    DDRA = 0x00;
    PORTA = 0xFF;  // config PA[7:0] as inputs, init to 1's
    DDRB = 0xFF;
    PORTB = 0x00;  // config PD[7:0] as outputs,
    DDRC = 0xFF;
    PORTC = 0x00;  // config PC[7:0] as outputs,
    DDRD = 0xFF;
    PORTD = 0x00;  // config PD[7:0] as outputs,

    static task task1, task2;
    task *tasks[] = {&task1, &task2};
    const unsigned short numTasks = sizeof(tasks) / sizeof(task *);

    const char start = -1;

    // Task 1 paddle1_tick
    task1.state = start;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &score_tick;

    task2.state = start;
    task2.period = 300;
    task2.elapsedTime = task1.period;
    task2.TickFct = &increase;

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
