/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 10  Exercise 2
 *      Exercise Description: 
 *      Note: since there was no direction as to how the button should be read,
 *            frequency can be adjusted by either pressing or holding the buttons.
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/rqWbAzyZ2Ns>
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/bit.h"
#include "../header/keypad.h"
#include "../header/scheduler.h"
#include "../header/sound.h"
#include "../header/timer.h"


// ------------------------------------------------------------------------------
unsigned char buttonPressed = 0x00; // shared variable 
// ------------------------------------------------------------------------------

enum sm1_states { sm1_init, sm1_pressed, sm1_wait };

int TickFct_keypadPress(int state) {
    unsigned char x = GetKeypadKey();
    switch (state) {
        case sm1_init:
            if (x == '\0') {
                state = sm1_init;
            } else {
                state = sm1_pressed;
            }
            break;
        case sm1_pressed:
            if (x == '\0') {  // keypad button released
                state = sm1_init;
            } else if (buttonPressed == x) {  // same keypad button still pressed
                state = sm1_pressed;
            } else {  // different button pressed
                state = sm1_wait;
            }
            break;
        case sm1_wait:
            if (x != '\0') {  // if keypad button not released
                state = sm1_wait;
            } else {  // keypbad button released
                state = sm1_init;
            }
            break;
        default:
            state = sm1_init;
            break;
    }
    switch (state) {  // State machine actions
        case sm1_init:
            buttonPressed = '\0';  // set buttonPressed to non pressed value
            break;
        case sm1_pressed:
            buttonPressed = x;  // save keypad button pressed
            break;
        case sm1_wait:
            buttonPressed = 0x00;
            break;
    }
    return state;
}

// ------------------------------------------------------------------------------
//  sm2 Variables 
unsigned char KEY[] = {'#', '1', '2', '3', '4', '5'};
unsigned char keySize = sizeof(KEY) / sizeof(KEY[0]);
unsigned int sm2_i = 0x00;        // iterates over KEY array
unsigned char output = 0x00;  // holds the value for PB0, locked = 0x00, unlocked = 0x01
// ------------------------------------------------------------------------------

enum sm2_States { sm2_init, sm2_set, sm2_wait, sm2_pressed, sm2_unlocked };

int TickFct_doorLock(int state) {
    unsigned char lockButton = (~PINB & 0x80);  // captures inputs at PB7

    switch (state) {  // Transitions
        case sm2_init:
            sm2_i = 0;
            if (buttonPressed == KEY[sm2_i]) {
                state = sm2_set;
                sm2_i++;
            }
            break;
        case sm2_set:
            if (buttonPressed == KEY[0]) {  // if(buttonPressed == '#')
                state = sm2_set;
            } else if (buttonPressed == '\0') {  // # released
                state = sm2_wait;
            } else {  // any other button is pressed
                state = sm2_init;
            }
            break;
        case sm2_wait:
            if (buttonPressed == KEY[sm2_i]) {
                state = sm2_pressed;
                sm2_i++;
            } else if (buttonPressed == '\0') {
                // no keypad pressed after a button press and release sequence
                state = sm2_wait;
            } else {  // any other key that's not 1, 2, 3, 4 or 5
                state = sm2_init;
            }
            break;
        case sm2_pressed:
            if (buttonPressed == '\0' && !(sm2_i < keySize)) { 
                state = sm2_unlocked;
            } else if (buttonPressed == '\0' && (sm2_i < keySize)) {  
                state = sm2_wait;
            } else if (buttonPressed != KEY[sm2_i-1]) {
                state = sm2_init;
            }
            break;
        case sm2_unlocked:
            if ( output && lockButton) {  // if unlocked and P7 pressed, lock
                state = sm2_init;
            }
            break;
        default:
            state = sm2_init;
            break;
    }  // Transitions

    switch (state) {  // State actions
        case sm2_init:
            sm2_i = 0;
            output = 0x00;
            break;
        case sm2_set:
            break;
        case sm2_wait:
            break;
        case sm2_pressed:
            break;
        case sm2_unlocked:
            output = 0x01;
            break;
        default:  // ADD default behaviour below
            break;
    }  // State actions

    PORTB = output;
    return state;
}


int main(void) {

    DDRA = 0x00; PORTA = 0xFF; // config PA[7:0] as inputs, 
    DDRB = 0x7F; PORTB = 0x80;  // config PB[7:4] as outputs, PB[3:0] inputs
    DDRC = 0xF0; PORTC = 0x0F;  // config PC[7:4] as inputs, PC[3:0] outputs

    static task task1, task2;
    task *tasks[] = {&task1, &task2};
    const unsigned short numTasks = sizeof(tasks) / sizeof(task *);

    const char start = -1;

    // Task 1 keypadPress
    task1.state = start;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &TickFct_keypadPress;

    // Task 2 doorLock
    task2.state = start;
    task2.period = 100;
    task2.elapsedTime = task2.period;
    task2.TickFct = &TickFct_doorLock;


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
        while (!TimerFlag) ;
        TimerFlag = 0;
    }
    return 0;  // Error: Program should not exit!
}
