/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 6  Exercise 3
 *      Exercise Description: SynchSM implementation for a system that increments 
 *               PORTB by 1 upto 9 while PINA0 is pressed or decrements PORTB by
 *               1 utill 0 while PINA1 is pressed. If both buttons are pressed
 *               (even if not initially simultaneously), PORTB is reset to 0
 *               When a button pressed, the count continues to increment or 
 *               decrement at a rate of once per second. 
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/f_9u1SMS2XE>
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/timer.h"

enum States { init, increment, inc_wait, reset, decrement, dec_wait } State;

unsigned char tmpB = 0x00; // temporary variable to write to PortB
unsigned char i = 0x00;

void Tick_counter() {
    unsigned char A0 = (~PINA & 0x01); // tmp variable to capture button press on A0
    unsigned char A1 = (~PINA & 0x02); // tmp variable to capture button press on A1

    switch(State) { // Transitions
        case -1:
            tmpB = 0x07;
            State = init;
            break;
        case init: 
            if (A0 && !A1) {
                State = increment;
            }
            else if (!A0 && A1) {
                State = decrement;
            }
            else if (A0 && A1) {
                State = reset;
            }
            else if (!A0 && !A1) {
                State = init;
            }
            break;
        case increment: 
            if (A0 && !A1) {
                State = inc_wait;
                i = 0;
            }
            else if (!A0 && !A1) {
                State = init;
            }
            else if (A0 && A1) {
                State = reset;
            }
            break;
        case inc_wait: 
            if ((A0 && !A1) && (i < 10)) {
                State = inc_wait;
                i++;
            }
            else if ((A0 && !A1)  && (i == 10)) {
                State = increment;
            }
            else if (!A0 && !A1) {
                State = init;
            }
            else if (A0 && A1) {
                State = reset;
            }
            break;
        case reset: 
            if (!A0 && !A1) {
                State = init;
            }
            else if (A0 && A1) {
                State = reset;
            }
            break;
        case decrement: 
            if (!A0 && A1) {
                State = dec_wait;
                i = 0;
            }
            else if (!A0 && !A1) {
                State = init;
            }
            else if (A0 && A1) {
                State = reset;
            }
            break;
        case dec_wait: 
            if ((!A0 && A1) && (i < 10)) {
                State = dec_wait;
                i++;
            }
            else if ((!A0 && A1) && (i == 10)) {
                State = decrement;
            }            
            else if (!A0 && !A1) {
                State = init;
            }
            else if (A0 && !A1) {
                State = reset;
            }
            break;
        default:
            State = init;
   } // Transitions

    switch(State) { // State actions
        case init:
            break;
        case increment:
            if(tmpB < 9) tmpB++;
            break;
        case inc_wait:
            break;
        case reset:
            tmpB = 0x00;
            break;
        case decrement:
            if(tmpB > 0) tmpB--;
            break;
        case dec_wait:
            break;
        default: 
        break;
    } // State actions

}

int main() {
    DDRA = 0x00; PORTA = 0xFF; // configure port A[7:0] as inputs,
    DDRB = 0xFF; PORTB = 0x00; // Configure port B[7:0] as outputs, initialize to 0s

    const unsigned int cnt = 100;
    TimerSet(cnt);
    TimerOn();
    
    State = -1; // Initial state
    tmpB = 0; // Init outputs
    PORTB = tmpB;

    while(1) {
        Tick_counter();
        PORTB = tmpB;
        while(!TimerFlag); // Wait periodcounter second
        TimerFlag = 0;
    } 
    return 0;
}