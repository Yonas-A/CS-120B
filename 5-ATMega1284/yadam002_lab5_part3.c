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
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; // TImerISR() sets this to 1. we should clear to 0;

// Internal variables for mapping AVR's ISR to our cleaner TImerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms
unsigned long _avr_timer_cntcurr = 0; // current internal count of 1ms ticks


void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; // bit3 = 0:  CTC mode (clear timer on compare)
							 // bit[2:0] = 011: pre-scaler /64
							 // 00001011: 0x0B
							 // So, 8MHz clock or 8,000,00 /64 = 125,00 ticks/s
							 // Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A
	OCR1A = 125; // Timer interrupt will be generated when TCNT1 == OCR1A
				 // We want a 1ms tick. 0.001s * 125,000 ticks/s = 125
				 // so when TCNT1 register equals 125, 
				 // 1ms has passed. Thus, we compare to 125.

	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	// Initialize avr counter
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TImerISR will be called every _avr_timer_cntcurr ms

	// Enable global interrupts
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00; //bit[2:0] = 000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	// CPU automatically calles when TCNT1 == OCR! ( every 1ms per TImerOn settings)
	_avr_timer_cntcurr--; // counter down to 0 rather than up to TOP

	if(_avr_timer_cntcurr == 0) {
		TimerISR(); // call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void main() {
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s

	TimerSet(1000);
	TimerOn();
	unsigned char tempB = 0x00;
	while(1) {

		// User code (i.e. synchSM calls)
		tempB = ~tempB; // Toggle PORTB
		PORTB = tempB;
		while(!TimerFlag); // wait 1 sec
		TimerFlag = 0;
		// 
	}
}