/*	Author: yadam002
 *      Partner(s) Name: Yonas Adamu
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif    

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRC = 0xFF; PORTC = 0X00; // Configure port C's 8 pins as outputs, initialize to 0s
    
    unsigned char cntavail = 0x00; // Temporary variable to hold the value of C
    unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
    
    while(1) {

        tmpA = PINA & 0x0F;  // tmpA is 0000AAAA
        
        PORTC = 0x00; // reset PORTC before each iteration. 
        cntavail = (PINA & 0x01) + ((PINA >> 1 )& 0x01) + ((PINA >> 2 )& 0x01) + ((PINA >> 3 )& 0x01); 
        PORTC = 0x04 - cntavail; //(cntavail == 0x00) ? PORTC + 0x01 : PORTC + 0x00;
    }
    // if PORTC 4 rightmost bits are all 1, set PC7 to 1 else set PC7 to 0
     PORTC = (cntavail == 0x00) ? PORTC | 0x80 : PORTC  ;
 
   return 0;
}
