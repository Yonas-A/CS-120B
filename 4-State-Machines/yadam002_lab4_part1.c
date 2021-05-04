/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"

enum LED_STATES { start, init, led2_on, wait, led1_on} state;

#endif

void Tick_Led() { 
    unsigned char A0 = PINA & 0x01;

    switch(state) { 
        case start:
            if (1) {
                state = init;
            }
            break;

        case init:
            if (A0) {
                state = led2_on;
            }
            else {
                state = init;
            }
            break;

        case led2_on:
            if (!A0) {
                state = wait;
            }
            else {
                state = led2_on;
            }
            break;

        case wait:
            if (A0) {
                state = led1_on;
            }
            else {
                state = wait;
            }
            break;

        case led1_on:
            if (!A0) {
                state = init;
            }
            else {
                state = led1_on;
            }
            break;
      
        default:
            state = init;
            break;
    }

    switch(state) { 
        case start:
         break;

        case init:
            PORTB = 0x01;
            break;
        case led2_on:
            PORTB = 0x02;
            break;
        case wait:
            PORTB = 0x02;
            break;
        case led1_on:
            PORTB = 0x01;
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs,

    state = start;
    
    while (1) {
       Tick_Led();
    } 
    return 1;
}