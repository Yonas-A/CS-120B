/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 6 Exercise 1
 *      Exercise Description: synch SM implementation of a system that lights one
 *               of three LEDs, connected to B0, B1, and B2, one LED per second
 *               in sequence, such that the lit LED appears to move (and wrap 
 *               around) (like PB0, PB1, PB2, PB0, PB1, PB2 ...) 
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/9RwJosHEoVk>
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "../header/timer.h"

enum states {init, start, next}state;

unsigned char tmpB = 0x00; // temporary variable to write to PortB

void Tick_Led() { 
    
   switch(state) { 
        case init:
            state = start;
            break;
        case start:
            state = next;
            break;
        case next:
            state = next;
            break;
        default:
            state = start;
            break;
    }
    switch(state) { 
        case init:

            break;
        case start:
            tmpB = 0x01;
            break;
        case next:
            if (tmpB==0x04) tmpB = 0x01;
            else tmpB = tmpB << 1;
            break;
        default:
            break;
    }
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00; // Configure port B[7:0] as outputs, initialize to 0s

    int ElapsedTime = 1000;

    TimerSet(ElapsedTime);
    TimerOn();

	state = init;
	tmpB = 0x00;

	while (1) {
		Tick_Led();
		PORTB = tmpB;
        while(!TimerFlag); // Wait ElapsedTime second
        TimerFlag = 0;
	}

	return 0;
}
