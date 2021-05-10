/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 8 Exercise 2
 *      Exercise Description: synch SM implementation of a system that outputs
 *               three different sounds with three different button presses. A0
 *               Button press on A0 outputs a different sound that both A1 and A2  
 * 
 *      NOTE: After an Endorsed suggestion from EDstem 
 *            <https://edstem.org/us/courses/5172/discussion/428661>, I have 
 *            my tick_scale function to hold the current scale if toggle was 
 *            turned of while scaling. Forexample if scale is at E5 before toggle 
 *            was turned off, it should still be at E5 when we turn toggle back on
 * 
 * 
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/wACQWTuYgSI>
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/timer.h"

//-----------------------------------------------------------------------------
//  Starter Code for using audio in atmega1284/atmega1284p
//-----------------------------------------------------------------------------
void set_PWM(double frequency) {
    static double current_frequency;

    if (frequency != current_frequency) {
        if (!frequency)
            TCCR3B &= 0x08;
        else
            TCCR3B |= 0x03;

        if (frequency < 0.954)
            OCR3A = 0xFFFF;
        else if (frequency > 31250)
            OCR3A = 0x0000;
        else
            OCR3A = (short)(8000000 / (128 * frequency)) - 1;

        TCNT3 = 0;
        current_frequency = frequency;
    }
}

void PWM_on() {
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

/*This code will be shared between state machines.*/

static const double MUSIC_NOTES[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};


unsigned char A0 = 0x00; // a variable to capture button press on A0
unsigned char A1 = 0x00; // a variable to capture button press on A1
unsigned char A2 = 0x00; // a variable to capture button press on A2

enum sm1_states { sm1_init, sm1_on, sm1_wait_on, sm1_off } sm1_state;

static int setToggle = 0;

void Tick_toggle() {

    A0 = (~PINA & 0x01);  

    switch (sm1_state) {
        case sm1_init:
            setToggle = 0;
            if (A0) {
                sm1_state = sm1_on;
            } else if (!A0) {
                sm1_state = sm1_init;
            } else {
                sm1_state = sm1_init;
            }
            break;
        case sm1_on:
            if (!A0) {
                sm1_state = sm1_wait_on;
            } else {
                sm1_state = sm1_on;
            }
            break;
        case sm1_wait_on:
            if (A0) {
                sm1_state = sm1_off;
            } else {
                sm1_state = sm1_wait_on;
            }
            break;
        case sm1_off:
            if (!A0) {  // for button pushes, release is quick so need to wait in off state
                sm1_state = sm1_init;
            } else {
                sm1_state = sm1_off;
            }
            break;
        default:
            sm1_state = sm1_init;
            break;
    }
    switch (sm1_state) {
        case sm1_init:
            setToggle = 0;
            PWM_off(); // turn sound off
            break;
        case sm1_on:
            setToggle = 1;
            PWM_on(); // turn sound on
            break;
        case sm1_wait_on:
            break;
        case sm1_off:
            setToggle = 0;
            PWM_off(); // turn sound off
            break;
    }
}


enum sm2_States { sm2_init,  sm2_down,  sm2_up, sm2_wait } sm2_state;

void Tick_scale() {
    A1 = (~PINA & 0x02);  // tmp var to capture button press on A1
    A2 = (~PINA & 0x04);  // tmp var to capture button press on A2

    static int i = 0;
    switch (sm2_state) {  // Transitions
        case -1:
            i = 0;
            set_PWM(MUSIC_NOTES[i]);
            sm2_state = sm2_init;
            break;
        case sm2_init:
            if (A1 && !A2 && setToggle) {
                sm2_state = sm2_up;
            } else if (!A1 && A2 && setToggle) {
                sm2_state = sm2_down;
            } else if(!setToggle) {
                i = 0;
                sm2_state = sm2_init;
            }
            break;
        case sm2_up:
            sm2_state = sm2_wait;
            break;
        case sm2_down:
            sm2_state = sm2_wait;
            break;
        case sm2_wait:
            if (A1 || A2) {
                sm2_state = sm2_wait;
            } else if (!A1 && !A2) {
                sm2_state = sm2_init;
            }
            break;
        default:
            sm2_state = sm2_init;
    }  // Transitions

    switch (sm2_state) {  // State actions
        case sm2_init:
            set_PWM(MUSIC_NOTES[i]);
            break;
        case sm2_up:
            if (i < 7) i++;
            set_PWM(MUSIC_NOTES[i]);
            break;
        case sm2_down:
            if (i > 0) i--;
            set_PWM(MUSIC_NOTES[i]);
            break;
        case sm2_wait:
            break;
        default:  // ADD default behaviour below
            i = 0;
            set_PWM(MUSIC_NOTES[i]);
            break;
    }  // State actions
}
int main() {
    DDRA = 0x00; PORTA = 0xFF;  // Configure port A[7:0] pins as inputs
    DDRB = 0xFF; PORTB = 0xBF;  // configure port B[7:0] pins as outputs, initialize to 0
        
    int ElapsedTime = 100;

    TimerSet(ElapsedTime);
    TimerOn();

    sm1_state = sm1_off;
    sm2_state = -1;

    while (1) {
        Tick_toggle();
        Tick_scale();
        while (!TimerFlag);
        TimerFlag = 0; // TimerFlag var from timer.h header file
    }    
    
}