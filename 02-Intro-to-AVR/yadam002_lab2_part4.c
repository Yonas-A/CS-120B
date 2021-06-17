#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {

    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs,
    DDRC = 0x00; PORTC = 0XFF; // Configure port C's 8 pins as inputs,
    DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs, initialize to 0s  

    unsigned short totalWeight = 0; // Temporary variable to hold A + B + C
    unsigned char diff = 0x00; // Temporary variable to hold the value of C - A
    unsigned char tmpA = 0x00, tmpB = 0x00, tmpC = 0x00, tmpD = 0x00;
    while(1) {

        tmpA = PINA; tmpB = PINB; tmpC = PINC;
        totalWeight = tmpA + tmpB + tmpC;

        // if total wights exceeds 140, set port PD0 to 1 else to 0
        PORTD = (totalWeight > 140 ) ?  0x01 : 0x00;

        // if A is greater than C, difference is A - C otherwise, difference is C - A
        diff = (tmpA > tmpC) ? tmpA - tmpC : tmpC - tmpA;

        // if difference between A and C exceeds 80, PD1 = 1 else PD1 = 0
        PORTD = (diff > 0x50) ? PORTD | 0x02 : PORTD;;

        // right shift totalWeight 2 places and set PD[7:2] to totalWeight
        tmpD = totalWeight >> 2;
        PORTD = PORTD | (tmpD & 0xFC);
    }
   return 0;
}