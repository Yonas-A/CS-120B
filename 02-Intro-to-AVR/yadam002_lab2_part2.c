/*	Author: yadam002
 *  Partner(s) Name: Yonas Adamu
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

    unsigned char cntavail = 0x00; // Temporary variable to hold total sum of 0's in PA[3:0]
    unsigned char tmpA = 0x00; // Temporary variable to hold the sum of PA[3:0]

    while(1) {
        // extract single bits of PA[3:0] and add them together 
        tmpA = (PINA & 0x01) + ((PINA >> 1 )& 0x01) + ((PINA >> 2 )& 0x01) + ((PINA >> 3 )& 0x01);
        
        // find the total number of 0's by subtracting total 1's from 4 
        cntavail = 0x04 - tmpA;
        PORTC = cntavail;
    }

   return 0;
}
