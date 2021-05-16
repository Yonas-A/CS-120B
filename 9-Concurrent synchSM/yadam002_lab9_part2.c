/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 9  Exercise 2
 *      Exercise Description: 
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/ijaWvvF79J4>
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/timer.h"


unsigned char threeLED = 0x00;     // captures the action to be performed on B[2:0]
unsigned char blinkingLED = 0x00;  // captures the action to be performed on B3
unsigned char tmpB = 0x00;         // captures the combined action of B[2:0] and B3

enum sm1_states { sm1_init, sm1_s0,  sm1_s1, sm1_s2 } sm1_state;

void Tick_ThreeLEDs() {
    switch (sm1_state) {
        case sm1_init:
            sm1_state = sm1_s0;
            break;
        case sm1_s0:
            sm1_state = sm1_s1;
            break;
        case sm1_s1:
            sm1_state = sm1_s2;
            break;
        case sm1_s2:
            sm1_state = sm1_s0;
            break;
        default:
            sm1_state = sm1_s0;
            break;
    }
    switch (sm1_state) {
        case sm1_init:
            break;
        case sm1_s0:
            threeLED = 0x01;
            break;
        case sm1_s1:
            threeLED = 0x02;
            break;
        case sm1_s2:
            threeLED = 0x04;
            break;
    }
}

enum sm2_states { sm2_init, sm2_s0, sm2_s1 } sm2_state;

void Tick_BlinkingLEDs() {
    switch (sm2_state) {
        case sm2_init:
            sm2_state = sm2_s0;
            break;
        case sm2_s0:
            sm2_state = sm2_s1;
            break;
        case sm2_s1:
            sm2_state = sm2_s0;
            break;
        default:
            sm2_state = sm2_init;
            break;
    }
    switch (sm2_state) {
        case sm2_init:
            break;
        case sm2_s0:
            blinkingLED = 0x01;  // turns Bit 3 on
            break;
        case sm2_s1:
            blinkingLED = 0x00;  // turns Bit 3 on
            break;
    }
}

enum combined_states { combined_init, combined_set } combined_state;

void Tick_CombineLEDs() {
    switch (combined_state) {
        case combined_init:
            combined_state = combined_set;
            break;
        case combined_set:
            combined_state = combined_set;
            break;
        default:
            combined_state = combined_init;
            break;
    }
    switch (combined_state) {
        case combined_init:
            tmpB = 0x00;
            break;
        case combined_set:
            tmpB = (blinkingLED << 3) | (threeLED);
            // left shift blinkLED's value 3 times to capture B3's value
            break;
    }
}

int main() {
    DDRB = 0xFF; PORTB = 0x00;  // configure port B[7:0] pins as outputs, initialize to 0

    int ThreeLEDsElapsedTime = 300;
    int BlinkingLEDsElapsedTime = 1000;
    int CombineLEDsElapsedTime = 1000;
    const unsigned long timerPeriod = 100;


    sm1_state = sm1_init;
    sm2_state = sm2_init;
    combined_state = combined_init;

    TimerSet(timerPeriod);
    TimerOn();
    PORTB = 0x00;

    while (1) {
        if (ThreeLEDsElapsedTime >= 1000) {
            Tick_ThreeLEDs();
            ThreeLEDsElapsedTime = 0;
        }
        if (BlinkingLEDsElapsedTime >= 1000) {
            Tick_BlinkingLEDs();
            BlinkingLEDsElapsedTime = 0;
        }
        if (CombineLEDsElapsedTime >= 300) {
            Tick_CombineLEDs();
            CombineLEDsElapsedTime = 0;
        }

        ThreeLEDsElapsedTime += timerPeriod;
        BlinkingLEDsElapsedTime += timerPeriod;
        CombineLEDsElapsedTime += timerPeriod;

        PORTB = tmpB;
        while (!TimerFlag);
        TimerFlag = 0;
    }
    return 1;    
}
