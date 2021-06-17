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

enum SM2_STATES { SM2_SMStart, SM2_init, SM2_lock, SM2_press_1, SM2_wait, SM2_unlock } SM2_STATE;

#endif

unsigned char tmpB = 0x00; // temporary var to write to PortC 

void Tick_Padlock() { 
    unsigned char A0 = PINA & 0x00; //temp var to hold PA0 
    unsigned char A1 = PINA & 0x01; //temp var to hold PA1 
    unsigned char A2 = PINA & 0x02; //temp var to hold PA2 
    unsigned char A7 = PINA & 0x07; //temp var to hold PA7

    switch(SM2_STATE) { 
      case SM2_SMStart:
         if (1) {
            SM2_STATE = SM2_init;
         }
         break;
      case SM2_init:
         if (1) {
            SM2_STATE = SM2_lock;
         }
         break;
      case SM2_lock:
         if (!A0&&!A1&&A2) {
            SM2_STATE = SM2_press_1;
         }
         else {
            SM2_STATE = SM2_lock;
         }
         break;
      case SM2_press_1:
         if (!A0 && !A1 && !A2) {
            SM2_STATE = SM2_wait;
         }
         else {
            SM2_STATE = SM2_press_1;
         }
         break;
      case SM2_wait:
         if (!A0 && A1 && !A2) {
            SM2_STATE = SM2_unlock;
         }
         else {
            SM2_STATE = SM2_wait;
         }
         break;
      case SM2_unlock:
         if (A7) {
            SM2_STATE = SM2_lock;
         }
         else {
            SM2_STATE = SM2_unlock;
         }
         break;         
      default:
         SM2_STATE = SM2_init;
         break;
   }
   switch(SM2_STATE) { 
      case SM2_SMStart:
         break;
      case SM2_init:
         break;
      case SM2_lock:
         tmpB = 0x00;
         break;
      case SM2_press_1:
         break;
      case SM2_wait:
         break;         
      case SM2_unlock:
         tmpB = 0x01;
         break;
   }
}



int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0xF0; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s

    tmpB = 0x00;

    SM2_STATE = SM2_SMStart;
	 while (1) {
        PORTC = tmpB;
        Tick_Padlock();
    } 
    return 1;
}