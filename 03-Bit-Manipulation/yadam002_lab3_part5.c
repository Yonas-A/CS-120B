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

    DDRB = 0x01; PORTB = 0xFE; // Configure port B to output on B[7:1], and input on B0
    DDRD = 0x00; PORTD = 0xFF; // Configure port D's 8 pins as input, initialize to 1s  

    unsigned short total = 0x00; // temporary var to hold PD[7:0] + PB0
    unsigned char tmpB = 0x00;  // temporary var to hold B[]
    unsigned char enabled = 0x00, disabled = 0x00;

    while (1) {

        // initialize temporary variables
        total = 0x0; tmpB = 0x00; enabled = 0x00; disabled = 0x00;

        // set tmpB to B0 using a mask, tmpB = 0000000b 
        tmpB = PINB & 0x01;

        // set total to D[7:0] and shift left one bit and add B0
        total = ( (PIND & 0xFF ) << 1 ) | tmpB;

        // if total is 70 or above, PB1 = xxxxxx1x, so set enabled to 0x02
        if(total > 0x45) enabled = 0x02;

        // total is above 5 but below 70, PB2 = xxxxxx1xx, so set enabled to 0x04
        else if(total > 0x05 && total < 0x46) disabled = 0x04;

        // total is below 6 PB[2:1] = xxxxxx00x, so set enabled to 0x04
        else if(total < 0x06) { enabled = 0x00; disabled = 0x00;}

        // set output on port B 
        PORTB = tmpB | (enabled | disabled);
    }
    return 1;
}