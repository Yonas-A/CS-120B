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
#endif

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs,
    DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as inputs,
    //DDRC = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs, initialize to 0s  

    unsigned char tmpA = 0x00, tmpB = 0x00 , tmpC = 0x00;

    while (1) {

        int i = 0;  tmpC = 0x00;
        for(i = 0; i < 8 ; i++) {
            tmpA = (PINA >> i) & 0x01;
            tmpB = (PINB >> i) & 0x01;
            tmpC += tmpA + tmpB;
        }
        PORTC = tmpC;

    }
    return 1;
}