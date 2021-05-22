/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 10  Exercise 4
 *      Exercise Description: 
 *      Note: since there was no direction as to how the button should be read,
 *            frequency can be adjusted by either pressing or holding the buttons.
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/VPqvtIHWCmQ>
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

// =============================================================================
// ------------------------------- sm2 variables -------------------------------

unsigned char KEY[] = {'#', '1', '2', '3', '4', '5'};
unsigned char keyLength = sizeof(KEY) / sizeof(KEY[0]);
unsigned int sm2_i = 0x00;    // iterates over KEY array
unsigned char output = 0x00;  // holds PB0's bit. locked = 0x00, unlocked = 0x01

// ------------------------------------------------------------------------------
// =============================================================================

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
            if (buttonPressed == '\0' && !(sm2_i < keyLength)) { 
                state = sm2_unlocked;
            } else if (buttonPressed == '\0' && (sm2_i < keyLength)) {  
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

// =============================================================================
// ------------------------------- sm3 variables -------------------------------

unsigned int sm3_i = 0;  // temporary variable to iterate over the arrays
unsigned int cnt = 0;
static const double NOTE[] = {329.63, 392.00, 440.00, 493.88};
static const double TIME[] = {3, 4, 2, 3};
const unsigned short melodySize = sizeof(NOTE) / sizeof(NOTE[0]);

// ------------------------------------------------------------------------------
// =============================================================================

enum sm3_states { sm3_init, sm3_set, sm3_play, sm3_count, sm3_wait };

int TickFct_doorbell(int state) {
    unsigned char A7 = (~PINA & 0x80); // capture button press on PA0

    switch (state) {  // Transitions
        case sm3_init:
            if (A7) {
                state = sm3_set;
            }
            break;
        case sm3_set:
            state = sm3_play;
            break;
        case sm3_play:
            state = sm3_count;
            break;
        case sm3_count:
            if (sm3_i >= melodySize  && !A7) {  // if button released go to init
                state = sm3_init;
            } else if (sm3_i >= melodySize && A7) {  // if button still pressed go to wait
                state = sm3_wait;
            } else if (!(sm3_i >= melodySize ) && cnt > 0) {
                state = sm3_count;
            } else if (!(sm3_i >= melodySize ) && cnt == 0) {
                state = sm3_play;
            }
            break;
        case sm3_wait:
            if (A7) {
                state = sm3_wait;
            } else if (!A7) {
                state = sm3_init;
            }
            break;
        default:
            state = sm3_init;
    }  // Transitions

    switch (state) {  // State actions
        case sm3_init:
            sm3_i = 0;
            cnt = 0;
            PWM_off();
            break;
        case sm3_set:
            PWM_on();
            break;
        case sm3_play:
            set_PWM(NOTE[sm3_i]);
            cnt = TIME[sm3_i];
            if (sm3_i < melodySize )
                sm3_i++;
            break;
        case sm3_count:
            set_PWM(NOTE[sm3_i]);
            if (cnt > 0)
                cnt--;
            break;
        case sm3_wait:
            PWM_off();
            break;
        default:  // ADD default behaviour below
            break;
    }  // State actions

    return state;
}


// =============================================================================
// ------------------------------- sm4 variables -------------------------------

static int timeCounter = 0x00;
static int a2SecondsWait = 10;
static int sm4_i = 0;
static const int NEW_KEY_LENGTH = 4;
static char newKey[4];  // temporarily stores  user entered key sequences till the key is

////////////////////////////////////////////////////////////////////////////////
// helper function that saves the newest keypad sequence in the array.
void saveCombination() {
    unsigned int i = 0x00;
    for (i = 0; i < 4; i++) {
        KEY[i + 1] = newKey[i];
    }
    keyLength = sizeof(KEY) / sizeof(KEY[0]) - 1;  // accounting for a less key sequence.
    return;
}
// -----------------------------------------------------------------------------
// =============================================================================

enum sm4_States { sm4_init, sm4_start, sm4_waitPress, sm4_waitRelease, sm4_reEnterCombo,
                  sm4_releaseCombo, sm4_save };

int TickFct_changeKey(int state) {
    unsigned char x = GetKeypadKey();  // get keypad button input
    unsigned char B7 = (~PINB & 0x80); // capture PB7 button input

    switch (state) {
        case sm4_init:
            if (x == '*' && B7) {  // if lock and * pressed, ready to enter new combo
                state = sm4_start;
            }
            break;
        case sm4_start:
            if (x == '\0' && !B7) {  
                // if lock button released and * released, waiting to accept new combination
                state = sm4_waitPress; 
            }
            break;
        case sm4_waitPress:
            if (x != '\0') {
                state = sm4_waitRelease;
                newKey[sm4_i] = x;
                sm4_i++;
            }
            break;
        case sm4_waitRelease:
            if (x == '\0' && sm4_i < NEW_KEY_LENGTH) {
                state = sm4_waitPress;
            } else if (x == '\0' && !(sm4_i < NEW_KEY_LENGTH)) {
                state = sm4_reEnterCombo;
                sm4_i = 0; // reset i to count for re-entry of the combination
            }
            break;
        case sm4_reEnterCombo:
            if (x == '\0' && timeCounter < a2SecondsWait) {
                state = sm4_reEnterCombo;
                timeCounter++;
            } else if (x == newKey[sm4_i] && timeCounter <= a2SecondsWait) {
                state = sm4_releaseCombo;
                timeCounter++;
                sm4_i++;
            } else if ((x != newKey[sm4_i]) || !(timeCounter <= a2SecondsWait)) {
                state = sm4_init;
            }
            break;
        case sm4_releaseCombo:
            // if button released & not full sequence and less than 2 seconds since full new sequence input, wait for next input
            if (x == '\0' && sm4_i < NEW_KEY_LENGTH && timeCounter <= a2SecondsWait) {
                state = sm4_reEnterCombo;
                timeCounter++;
            } 
            // if button released & not full sequence but more than 2 seconds since full new sequence input, discard input, go to init
            else if (x == '\0' && sm4_i < NEW_KEY_LENGTH && !(timeCounter < a2SecondsWait)) {
                state = sm4_init;
            } 
            // if button pressed and less than 2sec since full new sequence input, wait for release
            else if (x != '\0' && timeCounter < a2SecondsWait) {
                state = sm4_releaseCombo;
                timeCounter++;
            } 
            // if button released, full sequence reentered and less than 2 seconds since full new sequence input, save this new full sequence
            else if ((x == '\0') && !(sm4_i < NEW_KEY_LENGTH) && (timeCounter <= a2SecondsWait)) {
                state = sm4_save;
            }
            break;
        case sm4_save:
            state = sm4_init;
            break;
        default:
            state = sm4_init;
            break;
    }  // Transitions

    switch (state) {  // State actions
        case sm4_init:
            sm4_i = 0;
            timeCounter = 0;
            break;
        case sm4_start:
            break;
        case sm4_waitPress:
            break;
        case sm4_waitRelease:
            break;
        case sm4_reEnterCombo:
            // PORTB = PINB | 0x04;
            break;
        case sm4_releaseCombo:
            // PORTB = PINB | 0x08;
            break;
        case sm4_save:
            saveCombination();
            break;
        default:  // ADD default behaviour below
            break;
    }  // State actions
    return state;
}

int main(void) {

    DDRA = 0x00; PORTA = 0xFF; // config PA[7:0] as inputs, 
    DDRB = 0x7F; PORTB = 0x80;  // config PB[7:4] as outputs, PB[3:0] inputs
    DDRC = 0xF0; PORTC = 0x0F;  // config PC[7:4] as inputs, PC[3:0] outputs

    static task task1, task2, task3, task4;
    task *tasks[] = {&task1, &task2, &task3, &task4};
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

    // Task 3 doorbell 
    task3.state = start;
    task3.period = 200;
    task3.elapsedTime = task3.period;
    task3.TickFct = &TickFct_doorbell;

    // Task 4 changeKey 
    task4.state = start;
    task4.period = 200;
    task4.elapsedTime = task4.period;
    task4.TickFct = &TickFct_changeKey;

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
