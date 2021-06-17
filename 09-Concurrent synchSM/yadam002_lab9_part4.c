/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 9  Exercise 4
 *      Exercise Description: 
 *      Note: since there was no direction as to how the button should be read,
 *            frequency can be adjusted by either pressing or holding the buttons.
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/fnN-G71zOJA>
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/timer.h"

unsigned char A0 = 0x00;  // tmp variable to capture button press on A0
unsigned char A1 = 0x00;  // tmp variable to capture button press on A1
unsigned char A2 = 0x00;  // tmp variable to capture button press on A1

unsigned char threeLED = 0x00;     // captures the action to be performed on B[2:0]
unsigned char blinkingLED = 0x00;  // captures the action to be performed on B3
unsigned char toggleSound = 0x00;  // captures the action to be performed on B4
unsigned int soundPeriod = 0x00;   // captures the adjusted period to ouput a lowe or higher frequency sound

unsigned char tmpB = 0x00;  // captures the combined action of B[2:0], B3 and B4

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

enum sm4_states { sm4_start,
                  sm4_init,
                  sm4_inc,
                  sm4_dec } sm4_state;

void Tick_AdjustFrequency() {
    A0 = (~PINA & 0x01);  // capture input button A0;
    A1 = (~PINA & 0x02);  // capture input button A1;

    switch (sm4_state) {
        case sm4_init:
            if (!A0 && !A1) {
                sm4_state = sm4_init;
            } else if (A0) {
                sm4_state = sm4_inc;
            } else if (A1) {
                sm4_state = sm4_dec;
            } else {
                sm4_state = sm4_init;
            }
            break;
        case sm4_inc:
            if (!A0) {
                sm4_state = sm4_init;
            } else {
                sm4_state = sm4_inc;
            }
            break;
        case sm4_dec:
            if (!A1) {
                sm4_state = sm4_init;
            } else {
                sm4_state = sm4_dec;
            }
            break;
        default:
            sm4_state = sm4_init;
            break;
    }
    switch (sm4_state) {
        case sm4_init:
            soundPeriod = soundPeriod;
            break;
        case sm4_inc:                               // increasing sound volume by decreasing the period the sound is toggled
            if (soundPeriod > 0x01) soundPeriod--;  // higher period, lower sound
            break;
        case sm4_dec:                               // decreasing sound volume by increasing the period the sound is toggled
            if (soundPeriod < 0x0A) soundPeriod++;  // lower period, higher sound
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
    DDRA = 0x00;
    PORTA = 0xFF;  // configure port A[7:0] pins as inputs
    DDRB = 0xFF;
    PORTB = 0x00;  // configure port B[7:0] pins as outputs, initialize to 0

    int ThreeLEDsElapsedTime = 300;
    int BlinkingLEDsElapsedTime = 1000;
    int ToggleSoundElapsedTime = 2;
    int AdjustFrequencyElapsedTime = 300;
    int CombineLEDsElapsedTime = 1;

    const unsigned long timerPeriod = 1;  // timer period that evenly divides the above periods
    soundPeriod = 0x05;                   // initialize frequency to 5

    sm1_state = sm1_init;
    sm2_state = sm2_init;
    sm3_state = sm3_init;
    sm4_state = sm4_init;
    combined_state = combined_init;

    PORTB = 0x00;

    TimerSet(timerPeriod);
    TimerOn();

    while (1) {
        if (ThreeLEDsElapsedTime >= 300) {
            Tick_ThreeLEDs();
            ThreeLEDsElapsedTime = 0;
        }
        if (BlinkingLEDsElapsedTime >= 1000) {
            Tick_BlinkingLEDs();
            BlinkingLEDsElapsedTime = 0;
        }
        if (ToggleSoundElapsedTime >= soundPeriod) {  // changing the frequency of pulse changes sound volume
            Tick_ToggleSound();
            ToggleSoundElapsedTime = 0;
        }
        if (AdjustFrequencyElapsedTime >= 300) {
            Tick_AdjustFrequency();
            AdjustFrequencyElapsedTime = 0;
        }
        if (CombineLEDsElapsedTime >= 1) {
            Tick_CombineLEDs();
            CombineLEDsElapsedTime = 0;
        }

        ThreeLEDsElapsedTime += timerPeriod;
        BlinkingLEDsElapsedTime += timerPeriod;
        ToggleSoundElapsedTime += timerPeriod;
        AdjustFrequencyElapsedTime += timerPeriod;
        CombineLEDsElapsedTime += timerPeriod;

        PORTB = tmpB;
        while (!TimerFlag)
            ;
        TimerFlag = 0;
    }
    return 1;
}
