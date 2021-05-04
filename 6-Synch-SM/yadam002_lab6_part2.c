/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 6  Exercise 2
 *      Exercise Description: synch SM implementation of a system that lights one
 *               of three LEDs, connected to B0, B1, and B2, one LED per second
 *               in sequence, such that the lit LED appears to move in like PB0, 
 *               PB1, PB2, PB1, PB0, PB1 ...sequence. if button PA0 is pressed, 
 *               the curently lit LED stays lit. When the button is released,
 *               the LED starts from B0
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/Y5LvzxacB08>
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/timer.h"

enum states {init, L0, L1_a, L2, L1_b, wait } state;

unsigned char tmpB = 0x00; // temporary variable to write to PortB

void Tick_LedGame() { 
    unsigned char A0 = (~PINA & 0x01); // temporary variable to capture button press
    
    switch(state) { 
        case init:
            state = L0;
            break;
        case L0:
            if(A0) {
                state = wait;
                tmpB = 0x01;
            }
            else{
                state = L1_a;
            }
            break;
        case L1_a:
            if(A0) {
                state = wait;
                tmpB = 0x02;
            }
            else{
                state = L2;
            }
            break;
        case L2:
            if(A0) {
                state = wait;
                tmpB = 0x04;
            }
            else{
                state = L1_b;
            }        
            break;
        case L1_b:
            if(A0) {
                state = wait;
                tmpB = 0x02;
            }
            else{
                state = L0;
            }
            break;         
        case wait:
            if(A0) {
                state = wait;
                tmpB = tmpB;
            }
            else {
                state = init;
            }
        default:
            break;
    }
    switch(state) { 
        case init:
            
            break;
        case L0:
            tmpB = 0x01;
            break;
        case L1_a:
            tmpB = 0x02;
            break;
        case L2:
            tmpB = 0x04;
            break;
        case L1_b:
            tmpB = 0x02;
            break;
        case wait:
            tmpB = tmpB;
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // configure port A[7:0] as inputs,
	DDRB = 0xFF; PORTB = 0x00; // Configure port B[7:0] as outputs, initialize to 0s

    int ElapsedTime = 300;

    TimerSet(ElapsedTime);
    TimerOn();

	state = init;
	//tmpB = 0x00;

	while (1) {
		Tick_LedGame();
		PORTB = tmpB;
        while(!TimerFlag); // Wait ElapsedTime second
        TimerFlag = 0;
	}

	return 0;
}
