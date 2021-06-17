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
    DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s  
    DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s  

    unsigned char tmp1 = 0x00, tmp2 = 0x00;

    while (1) {

        // take the lower nibble of PINA, right shift 4 times
        tmp1 = (PINA & 0x0F) << 4;

        // take the upper nibble of PINA, left shift 4 times
        tmp2 = (PINA & 0xF0) >> 4;

        PORTB = tmp2; // upper nibble of PINA to lower nibble of B
        PORTC = tmp1; // lower nibble of PINA to upper nibble of C
    }
    return 1;
}