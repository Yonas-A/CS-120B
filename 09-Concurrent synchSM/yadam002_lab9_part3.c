/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 9  Exercise 3
 *      Exercise Description: 
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/dpNKXiANmAs>
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/timer.h"


// unsigned char A0 = 0x00;  // tmp variable to capture button press on A0
// unsigned char A1 = 0x00;  // tmp variable to capture button press on A1
unsigned char A2 = 0x00;  // tmp variable to capture button press on A1

unsigned char threeLED = 0x00;     // captures the action to be performed on B[2:0]
unsigned char blinkingLED = 0x00;  // captures the action to be performed on B3
unsigned char toggleSound = 0x00;  // captures the action to be performed on B4

unsigned char tmpB = 0x00;         // captures the combined action of B[2:0], B3 and B4

enum sm1_states { sm1_init,
                  sm1_s0,
                  sm1_s1,
                  sm1_s2 } sm1_state;

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

enum sm2_states { sm2_init,
                  sm2_s0,
                  sm2_s1 } sm2_state;

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


enum sm3_states { sm3_init,
                  sm3_s0,
                  sm3_s1 } sm3_state;

void Tick_ToggleSound() {

    A2 = (~PINA & 0x04);  // capture input button A2;

    switch (sm3_state) {
        case sm3_init:
            if (A2) {
                sm3_state = sm3_s0;
            } else {
                sm3_state = sm3_init;
            }
            break;
        case sm3_s0:
            if (!A2) {
                sm3_state = sm3_init;
            } else {
                sm3_state = sm3_s1;
            }
            break;
        case sm3_s1:
            if (!A2) {
                sm3_state = sm3_init;
            } else {
                sm3_state = sm3_s0;
            }
            break;
        default:
            sm3_state = sm3_init;
            break;
    }
    switch (sm3_state) {
        case sm3_init:
            toggleSound = 0x00;
            break;
        case sm3_s0:
            toggleSound = 0x01;
            break;
        case sm3_s1:
            toggleSound = 0x00;
            break;
    }
}


enum combined_states { combined_init,
                  combined_set } combined_state;

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
            tmpB = (blinkingLED << 3) | (threeLED) | (toggleSound << 4);
            // left shift blinkLED's value 3 times to capture B3's value
            break;
    }
}

int main() {

    DDRA = 0x00; PORTA = 0xFF;  // configure port A[7:0] pins as inputs
    DDRB = 0xFF; PORTB = 0x00;  // configure port B[7:0] pins as outputs, initialize to 0
    
    int ThreeLEDsElapsedTime = 300;
    int BlinkingLEDsElapsedTime = 1000;
    int ToggleSoundElapsedTime = 2;
    int CombineLEDsElapsedTime = 1000;
    const unsigned long timerPeriod = 1;

    sm1_state = sm1_init;
    sm2_state = sm2_init;
    sm3_state = sm3_init;
    combined_state = combined_init;

    TimerSet(timerPeriod);
    TimerOn();

    PORTB = 0x00;

    while (1) {
        if (ThreeLEDsElapsedTime >= 300) {
            Tick_ThreeLEDs();
            ThreeLEDsElapsedTime = 0;
        }
        if (BlinkingLEDsElapsedTime >= 1000) {
            Tick_BlinkingLEDs();
            BlinkingLEDsElapsedTime = 0;
        }
        if (ToggleSoundElapsedTime >= 2) {
            Tick_ToggleSound();
            ToggleSoundElapsedTime = 0;
        }
        if (CombineLEDsElapsedTime >= 2) {
            Tick_CombineLEDs();
            CombineLEDsElapsedTime = 0;
        }

        ThreeLEDsElapsedTime += timerPeriod;
        BlinkingLEDsElapsedTime += timerPeriod;
        ToggleSoundElapsedTime += timerPeriod;
        CombineLEDsElapsedTime += timerPeriod;

        PORTB = tmpB;
        while (!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
