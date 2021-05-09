/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 8  Exercise 1
 *      Exercise Description: 
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/8ZxncQ36UEY>
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//-----------------------------------------------------------------------------
//  Starter Code for using audio in atmega1284/atmega1284p
//-----------------------------------------------------------------------------
void set_PWM(double frequency) {
   static double current_frequency;

   if (frequency != current_frequency) {
        if (!frequency)
            TCCR3B &= 0x08;
        else
            TCCR3B |= 0x03;

        if (frequency < 0.954)
            OCR3A = 0xFFFF;
        else if (frequency > 31250)
            OCR3A = 0x0000;
        else
            OCR3A = (short)(8000000 / (128 * frequency)) - 1;

        TCNT3 = 0;
       current_frequency = frequency;
    }
}

void PWM_on() {
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

static const double MUSIC_NOTES[] = {261.63, 349.23, 523.25};

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;  // Configure port A[7:0] pins as inputs
    DDRB = 0xFF; PORTB = 0xBF;  // configure port B[7:0] pins as outputs, initialize to 0

    unsigned char A0 = 0x00;  // tmp variable to capture button press on A0
    unsigned char A1 = 0x00;  // tmp variable to capture button press on A1
    unsigned char A2 = 0x00;  // tmp variable to capture button press on A1
    
    PWM_on();

    while (1) {
    
        A0 = (~PINA & 0x01);  
        A1 = (~PINA & 0x02);
        A2 = (~PINA & 0x04);

        if (A0 && !A1 && !A2) {
            set_PWM(MUSIC_NOTES[0]);
        } else if (!A0 && A1 && !A2) {
            set_PWM(MUSIC_NOTES[1]);
        } else if (!A0 && !A1 && A2) {
            set_PWM(MUSIC_NOTES[2]);
        } else {
            set_PWM(0);
        }

    }
    return 1;
}