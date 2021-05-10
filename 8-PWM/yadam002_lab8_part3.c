/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 8 Exercise 3
 *      Exercise Description: synchSM implementation of a system that plays a 
 *               short five second melody when a button is pressed. Once button
 *               is pressed, melody should play until completion. A button press
 *               during this won't interrupt any action. If button is pressed and
 *               held, melody finishes and should wait until button is released
 *               and pressed again to play the melody
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/UDZjTAmq7OA>
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/timer.h"

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

/*Define user variables and functions for this state machine here.*/
static const double NOTE[] = {329.63, 392.00, 440.00, 493.88, 440.00, 440.0, 440.0, 392.00, 444.0};
static const double TIME[] = {6, 4, 8, 3, 7, 3, 1, 2, 1};

unsigned int i = 0;  // temporary variable to iterate over the arrays
unsigned int cnt = 0;

enum _states { _init, _set, _play, _count, _wait } _state;

unsigned char A0 = 0x00;  // a variable to capture button press on A0

Tick_melodyPlayer() {
    A0 = (~PINA & 0x01);

    switch (_state) {  // Transitions
        case _init:
            if (A0) {
                _state = _set;
            }
            break;
        case _set:
            _state = _play;
            break;
        case _play:
            _state = _count;
            break;
        case _count:
            if (i >= 8 && !A0) {  // if button released go to init
                _state = _init;
            } else if (i >= 8 && A0) {  // if button still pressed go to wait
                _state = _wait;
            } else if (!(i >= 8) && cnt > 0) {
                _state = _count;
            } else if (!(i >= 8) && cnt == 0) {
                _state = _play;
            }
            break;
        case _wait:
            if (A0) {
                _state = _wait;
            } else if (!A0) {
                _state = _init;
            }
            break;
        default:
            _state = _init;
    }  // Transitions

    switch (_state) {  // State actions
        case _init:
            i = 0;
            cnt = 0;
            PWM_off();
            break;
        case _set:
            PWM_on();
            break;
        case _play:
            set_PWM(NOTE[i]);
            cnt = TIME[i];
            if (i < 8)
                i++;
            break;
        case _count:
            set_PWM(NOTE[i]);
            if (cnt > 0)
                cnt--;
            break;
        case _wait:
            PWM_off();
            break;
        default:  // ADD default behaviour below
            break;
    }  // State actions
}

int main() {
    DDRA = 0x00;  PORTA = 0xFF;  // Configure port A[7:0] pins as inputs
    DDRB = 0xFF;  PORTB = 0xBF;  // configure port B[7:0] pins as outputs, initialize to 0

    const unsigned int ElapsedTime = 120;

    TimerSet(ElapsedTime);
    TimerOn();

    _state = _init;  // Initial state

    while (1) {
        Tick_melodyPlayer();
        while (!TimerFlag);
        TimerFlag = 0;  // TimerFlag var from timer.h header file
    }
}