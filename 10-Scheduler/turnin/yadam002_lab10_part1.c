/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 9  Exercise 4
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
#include "../header/scheduler.h"
#include "../header/timer.h"

unsigned char GetKeypadKey() {

    PORTC = 0xEF; // Enable col 4 with 0, disable others with 1's
    asm("nop");   // a delay to allow PORTC to stabilize before checking
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

unsigned char output = 0x00;
unsigned char LOOKUP[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 
                 'B', 'C', 'D', '*', '#'};

enum sm_states { off, on };

int TickFct_lightUP(int state) {
    unsigned char x = GetKeypadKey();
    switch (state) {
        case off:
            state = off;
            for (int i = 0; i < 16; i++) {
                if (x == LOOKUP[i]) {
                    state = on;
                    break;
                }
            }
            break;
        case on:
            state = off;
            for (int i = 0; i < 16; i++) {
                if (x == LOOKUP[i]) {
                    state = on;
                    break;
                }
            }
            break;

        default:
            state = off;
            break;
    }
    switch (state) {  // State machine actions
        case off:
            output = 0x00;
            break;
        case on:
            output = 0x80;  // toggle PB7
            break;
    }
    PORTB = output;
    return state;
}

int main(void) {

    DDRB = 0xFF; PORTB = 0x00;  // PORTB set to output, outputs init 0s
    DDRC = 0xF0; PORTC = 0x0F;  // PC[7:4] outputs init 0s, PC[3:0] inputs init 1s
    
    static task task1;
    task *tasks[] = {&task1};

    const char start = -1;

    task1.state = start;                       
    task1.period = 200;                         
    task1.elapsedTime = task1.period;          
    task1.TickFct = &TickFct_lightUP;          // Function pointer

    unsigned long GCD = tasks[0]->period;

    TimerSet(GCD);
    TimerOn();

    while (1) {
    
        if(tasks[0]->elapsedTime == tasks[0]->period){ 
            tasks[0]->state = tasks[0]->TickFct(tasks[0]->state);  
            tasks[0]->elapsedTime = 0;   
        }
        tasks[0]->elapsedTime += GCD;    
        
        while (!TimerFlag){;}
        TimerFlag = 0;
    }
    return 0;  // Error: Program should not exit!
}
