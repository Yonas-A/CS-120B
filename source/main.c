/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *		Demo Link: < >
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"

unsigned int i = 0;
enum States {init, wait0, Inc, wait1, reverse, wait3, Dec, wait4 } State;

#endif

unsigned char tmpB = 0x00; // temporary var to write to PortB

TickFct_Led() {
	unsigned char A0 = ~PINA & 0x01; // temporary var to hold PA0 
			
	switch(State) { // Transitions
        case init: 
            State = wait0;
        	break;
		case wait0: 
			if (A0 && i < 6) {
				State = Inc;
			}
			else if (A0 && i == 6) {
				State = reverse;
			}
			else if (!A0) {
				State = wait0;
			}
			break;
		case Inc: 
			State = wait1;
			break;
		case wait1: 
			if (A0) {
				State = wait1;
			}
			else if (!A0) {
				State = wait0;
			}
			break;
		case reverse: 
			if (!A0) {
				State = wait3;
			}
			else if (A0) {
				State = reverse;
			}
			break;
		case wait3: 
			if (!A0) {
				State = wait3;
			}
			else if (A0 && i > 0) {
				State = Dec;
			}
			else if (A0 && i == 0) {
				State = wait1;
				tmpB = 0x00;
			}
			break;
		case Dec: 
			State = wait4;
			break;
		case wait4: 
			if (!A0) {
				State = wait3;
			}
			else if (A0) {
				State = wait4;
			}
			break;
		default:
			State = init;
   } // Transitions

   	switch(State) { // State actions
		case init:
			tmpB = 0x00;
			i = 0x00;
			break;
		case wait0:
			break;
		case Inc:
			tmpB = (tmpB << 1 ) | 0x01;
			i++;
			break;
		case wait1:
			break;
		case reverse:
			tmpB = 0x00;
			i = 6;
			break;
		case wait3:
			break;
		case Dec:
			tmpB = (tmpB  >> 1) | 0x20;
			--i;
			break;
		case wait4:
			break;
		default: // ADD default behaviour below
		break;
   } // State actions

}


int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s

	State = init;
	tmpB = 0x00;

	while (1) {
		// tmpB = 0x00;
		TickFct_Led();
		PORTB = tmpB;
	}

	return 0;
}
