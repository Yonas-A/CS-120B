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
    DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s  

    unsigned char tmpA = 0x00; // a temporary var. to hold PA[3:0] values
    unsigned char icon = 0x00; // a temporary var. to indicate PA is 4 or less
    unsigned char level = 0x00; // a temporary var. to hold fule level

    while (1) {

        // initialize values
        PORTC = 0x00; icon = 0x00; level = 0x00;
        
        // set tmpA to A[3:0]
        tmpA = PINA & 0x0F;

        // tmpA is 1 or 2,            PC5 = xx1xxxxx so set level to (0x20)
        if(tmpA == 0x01 || tmpA == 0x02) level = 0x20;

        // tmpA is 3 or 4,        PC[5:4] = xx11xxxx so set level to (0x30)
        else if(tmpA == 0x03 || tmpA == 0x04) level = 0x30;

        // tmpA is 5 or 6,        PC[5:3] = xx111xxx so set level to (0x38)
        else if(tmpA == 0x05 || tmpA == 0x06) level = 0x38;

        // tmpA is 7 or 8 or 9,   PC[5:2] = xx1111xx so set level to (0x3C)
        else if(tmpA == 0x07 || tmpA == 0x08 || tmpA == 0x09) level = 0x3C;

        // tmpA is 10 or 11 or 12 PC[5:1] = xx11111x so set level to (0x3E)
        else if(tmpA == 0x0A || tmpA == 0x0B || tmpA == 0x0C) level = 0x3E;

        // tmpA is 13 or 14 or 15 PC[5:0] = xx111111 so set level to (0x3F)
        else if(tmpA == 0x0D || tmpA == 0x0E || tmpA == 0x0F) level = 0x3F;


        // if tmpA is less than 5 PC6 = x1xxxxxx, so set icon to 0x40
        if(tmpA < 0x05) icon = 0x40;

        PORTC = level | icon;
    }
    return 1;
}