/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 10  Exercise 0
 *      Exercise Description: 
 *      Note: since there was no direction as to how the button should be read,
 *            frequency can be adjusted by either pressing or holding the buttons.
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <no demo required>
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/bit.h"

unsigned char GetKeypadKey() {

    PORTC = 0xEF; // Enable col 4 with 0, disable others with 1's
    asm("nop"); // add a delay to allow PORTC to stabilize before checking
    if(GetBit(PINC,0) == 0) { return ('1'); } 
    if(GetBit(PINC,1) == 0) { return ('4'); } 
    if(GetBit(PINC,2) == 0) { return ('7'); } 
    if(GetBit(PINC,3) == 0) { return ('*'); } 

    // check keys in col 2
    PORTC = 0xDF; 
    asm("nop"); 
    if(GetBit(PINC,0) == 0) { return ('2'); }
    if(GetBit(PINC,1) == 0) { return ('5'); } 
    if(GetBit(PINC,2) == 0) { return ('8'); } 
    if(GetBit(PINC,3) == 0) { return ('0'); } 

    // check keys in col 3
    PORTC = 0xBF; 
    asm("nop");
    if(GetBit(PINC,0) == 0) { return ('3'); }
    if(GetBit(PINC,1) == 0) { return ('6'); } 
    if(GetBit(PINC,2) == 0) { return ('9'); } 
    if(GetBit(PINC,3) == 0) { return ('#'); }     
    
    // check keys in col 4
    PORTC = 0x7F; 
    asm("nop");
    if(GetBit(PINC,0) == 0) { return ('A'); }
    if(GetBit(PINC,1) == 0) { return ('B'); } 
    if(GetBit(PINC,2) == 0) { return ('C'); } 
    if(GetBit(PINC,3) == 0) { return ('D'); }   
    
    return ('\0'); // default value
}

int main(void) {
    unsigned char x;
    DDRB = 0xFF; PORTB = 0x00;  // PORTB set to output, outputs init 0s
    DDRC = 0xF0; PORTC = 0x0F;  // PC[7:4] outputs init 0s, PC[3:0] inputs init 1s

    while (1) {
        x = GetKeypadKey();
        switch (x) {
            case '\0': PORTB = 0x1F; break; // all 5 LED's on

            case '1': PORTB = 0x01; break;  // hex equivalent 
            case '2': PORTB = 0x02; break;
            case '3': PORTB = 0x03; break;
            case '4': PORTB = 0x04; break;
            case '5': PORTB = 0x05; break;
            case '6': PORTB = 0x06; break;
            case '7': PORTB = 0x07; break;
            case '8': PORTB = 0x08; break;
            case '9': PORTB = 0x09; break;

            case 'A': PORTB = 0x0A; break;
            case 'B': PORTB = 0x0B; break;            
            case 'C': PORTB = 0x0C; break;
            case 'D': PORTB = 0x0D; break;
            
            case '*': PORTB = 0x0E; break;
            case '0': PORTB = 0x00; break;
            case '#': PORTB = 0x0F; break;

            default: PORTB = 0x1B; break;  // should never occur. MIddle LED off     
        }
    }
}