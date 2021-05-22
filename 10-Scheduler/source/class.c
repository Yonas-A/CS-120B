#include "RIMS.h"

volatile unsigned char TimerFlag = 0;  // ISR raises, main() lowers

void TimerISR() {
    TimerFlag = 1;
}

enum BL_States { BL_SMStart, BL_LedOff, BL_LedOn };

int TickFct_Blink(int state) {
    int nextState;
    switch (state) {  //Transitions
        case BL_SMStart:
            nextState = BL_LedOff;  //Initial state
            break;
        case BL_LedOff:
            nextState = BL_LedOn;
            break;
        case BL_LedOn:
            nextState = BL_LedOff;
            break;
        default:
            nextState = BL_SMStart;
            break;
    }

    switch (nextState) {  //State actions
        case BL_LedOff:
            B0 = 0;
            break;
        case BL_LedOn:
            B0 = 1;
            break;
        default:
            break;
    }
    return nextState;
}

enum SM1_STATES { SM1_SMStart, SM1_s0, SM1_s1, SM1_s2 };

int TickFct_ThreeLeds(int state) {
    int nextState;
    switch (state) {
        case SM1_SMStart:
            if (1) {
                nextState = SM1_s0;
            }
            break;
        case SM1_s0:
            if (1) {
                nextState = SM1_s1;
            }
            break;
        case SM1_s1:
            if (1) {
                nextState = SM1_s2;
            }
            break;
        case SM1_s2:
            if (1) {
                nextState = SM1_s0;
            }
            break;
        default:
            nextState = SM1_s0;
            break;
    }
    switch (nextState) {
        case SM1_SMStart:

            break;
        case SM1_s0:
            B5 = 1;
            B6 = 0;
            B7 = 0;

            break;
        case SM1_s1:
            B5 = 0;
            B6 = 1;
            B7 = 0;
            break;
        case SM1_s2:
            B5 = 0;
            B6 = 0;
            B7 = 1;
            break;
    }
    return nextState;
}

int TickFct_BlinkB3(int state) {
    B3 = !B3;
    return state;
}

// Blink task: State variable, enum, TickFct_Blink
// ThreeLED task: State variable, enum, TickFct_Blink

typedef struct task {
    int state;            // Current state of the task
    int (*TickFct)(int);  // Function to call for task's tick

    unsigned long period;       // Rate at which the task should tick
    unsigned long elapsedTime;  // Time since task's previous tick
} task;

task tasks[3];

void main() {
    int i;
    const int timerPeriod = 500;

    tasks[0].state = 0;
    tasks[0].TickFct = &TickFct_Blink;
    tasks[0].period = 1000;
    tasks[0].elapsedTime = tasks[0].period;

    tasks[1].state = 0;
    tasks[1].TickFct = &TickFct_ThreeLeds;
    tasks[1].period = 1000;
    tasks[1].elapsedTime = tasks[1].period;

    tasks[2].state = 0;
    tasks[2].TickFct = &TickFct_BlinkB3;
    tasks[2].period = 500;
    tasks[2].elapsedTime = tasks[2].period;

    B = 0;                  //Init outputs
    TimerSet(timerPeriod);  // GCD of periods
    TimerOn();

    while (1) {
        for (i = 0; i < 3; ++i) {
            if (tasks[i].elapsedTime >= tasks[i].period) {
                tasks[i].state = tasks[i].TickFct(tasks[i].state);
                tasks[i].elapsedTime = 0;
            }
            tasks[i].elapsedTime += timerPeriod;
        }

        while (!TimerFlag) ;  // Wait for BL's period
        TimerFlag = 0;  // Lower flag
    }
}
