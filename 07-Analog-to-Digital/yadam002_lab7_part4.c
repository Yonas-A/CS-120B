/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 7  Exercise 4
 *      Exercise Description: 
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/jFxzJbf1OUw>
 * 
 *      MAX value was determined by exposing the photoresistor to a flashlight 
 *      and reading the 10-bit binary values from the led and converting it to HEX.
 *      The LED's were connected in D1 D2 B7 B6 B5 B4 B3 B2 B1 B0  with D1 as 
 *      the most significant bit and B0 as the least significant bit. This was 
 *      recorded with the lab7 part2 setup.
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned const short MAX = 0x33C; 
// determined by exposing the photoresistor to a flashlight and recording 
// the 10-bit number from the led and converting the reading into HEX

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

    unsigned short my_short = 0x00; // holds the 10-bit ADC result
    unsigned short threshold = 0x00; // holds the threshold value of each of 8 LED

    ADC_init();
    threshold = (short)(MAX / 8);

    PORTB = 0x00;

    while (1) {
        my_short = ADC; 

        if( my_short < threshold*1) PORTB = 0x00;
        else if( (my_short > threshold*1) && (my_short <= threshold*2) ) PORTB = 0x01;
        else if( (my_short > threshold*2) && (my_short <= threshold*3) ) PORTB = 0x03;
        else if( (my_short > threshold*3) && (my_short <= threshold*4) ) PORTB = 0x07;
        else if( (my_short > threshold*4) && (my_short <= threshold*5) ) PORTB = 0x0F;
        else if( (my_short > threshold*5) && (my_short <= threshold*6) ) PORTB = 0x1F;
        else if( (my_short > threshold*6) && (my_short <= threshold*7) ) PORTB = 0x3F;
        else if( (my_short > threshold*7) && (my_short <= threshold*8) ) PORTB = 0x7F;
        else if( (my_short > threshold*8) && (my_short <= threshold*9) ) PORTB = 0xFF;    
    }
    return 1;
}