/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 7  Exercise 1
 *      Exercise Description: 
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/8STBSJhWirQ>
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init() {
    ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE);
    // ADEN: setting this bit enables analog-to-digital conversion.
    // ADSC: setting this bit starts the first conversion
    // ADATE: setting this bit enables auto-triggering. Since we are in 
    //        Free Running Mode, a new conversion will trigger whenever
    //        the previous conversion completes.
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Configure port A[7:0] pins as inputs
    DDRB = 0xFF; PORTB = 0x00; // configure port B[7:0] pins as outputs, initialize to 0    
    DDRD = 0xFF; PORTD = 0x00; // Configure port D[7:0] pins as outputs, initialize to 0s  

    unsigned short my_short = 0x00; // holds the 10-bit ADC result
    
    ADC_init();
    while (1) {
        my_short = ADC;    
        PORTB = (char)my_short; // set the lower 8 bits to PORTB
        PORTD = (char)(my_short >> 4); // Set the upper 2 bits on PORTD
    }
    return 1;
}