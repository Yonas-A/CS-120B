/*      Author: yadam002
 *      Partner(s) Name: Yonas
 *      Lab Section:
 *      Assignment: Lab 11  Exercise 1
 *      Exercise Description: 
 *      Pong game with an ai vs player mode with no modification
 *  
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo Link: <https://youtu.be/MdR3R9sP-M0>
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../header/analog_to_digital.h"
#include "../header/scheduler.h"
#include "../header/timer.h"
#include "../header/utility.h"

// =============================================================================
// ----------------------------- SHARED VARIABLES -----------------------------
const unsigned short ANALOG_HIGH = 0x321;
const unsigned short ANALOG_LOW = 0x190;

static unsigned char pd1 = 0xFD;  // holds the current row of left_paddle being displayed by display_tick
static unsigned char pd2 = 0xFD;  // holds the current row of right_paddle being displayed by display_tick

static unsigned char player1Score = 0x00;  // holds the score for player1/ left side player
static unsigned char player2Score = 0x00;  // holds the score for player2/ right side player

static bool startGame = false;  // holds if the game has started or not
static bool endGame = false;    // holds if the game has ended or not

static bool player1Win = false;  // holds if player1 won the current game
static bool player2Win = false;  // holds if player2 won the current game

static paddle left_paddle = {0xFD, 0xFB, 0xF7};   // the left side paddle
static paddle right_paddle = {0xFD, 0xFB, 0xF7};  // the right side paddle

static pos curr = {0xFB, 0x08};  // holds the current position of the ball on the LED matrix
static pos prev = {0xFB, 0x10};  // holds the previous position of the ball on the LED matrix

// =============================================================================
// =============================================================================
/**
 * Resets every variable to a default value
 */
void reset() {
    pd1 = pd2 = 0xFD;
    curr = updatePosition(curr, 0xFB, 0x08);
    prev = updatePosition(prev, 0xFB, 0x10);
    left_paddle = updatePaddle(left_paddle, 0xFD, 0xFB, 0xF7);
    right_paddle = updatePaddle(right_paddle, 0xFD, 0xFB, 0xF7);
    startGame = endGame = false;
    player1Win = player2Win = false;
}
// =============================================================================
// =============================================================================

// =============================================================================
// ----------------------- variables for ai_paddle_tick -----------------------

enum ai_states { ai_pd_init,
                 ai_pd_move };

int ai_paddle_tick(int state) {
    unsigned int random = next(0, 4);  // holds a random number between 0 and 4
    switch (state) {
        case ai_pd_init:
            if (!endGame) {
                state = ai_pd_move;
            }
            break;
        case ai_pd_move:
            state = ai_pd_init;
            break;
        default:
            state = ai_pd_init;
            break;
    }
    switch (state) {
        case ai_pd_init:
            break;
        case ai_pd_move:
            if (random == 1 && left_paddle.top < TOP) {
                // if we're not at the top, move left_paddle up a row
                left_paddle = moveUp(left_paddle);
                pd1 = left_paddle.top;
            } else if (random == 2 && left_paddle.top < 0xFD) {
                // if we're not at the top, move left_paddle up a row
                left_paddle = moveUp(left_paddle);
                pd1 = left_paddle.top;
            } else if (random == 3 && left_paddle.bottom > BOTTOM) {
                // if we're not at the top, move left_paddle up a row
                left_paddle = moveDown(left_paddle);
                pd1 = left_paddle.top;
            } else if (random == 4 && left_paddle.bottom > 0xF7) {
                // if we're not at the bottom, move left_paddle up a row
                left_paddle = moveDown(left_paddle);
                pd1 = left_paddle.top;
            }
            break;
        default:
            break;
    }
    return state;
}

// =============================================================================
// --------------------------- variables for pd2_tick --------------------------

enum pd2_states { pd2_init,
                  pd2_up,
                  pd2_down };

int paddle2_tick(int state) {
    unsigned short joystick_input = ADC;

    switch (state) {
        case pd2_init:
            if (joystick_input > ANALOG_HIGH) {
                state = pd2_up;
            } else if (joystick_input < ANALOG_LOW) {
                state = pd2_down;
            }
            break;
        case pd2_up:
            if (joystick_input > ANALOG_HIGH) {
                state = pd2_up;
            } else {
                state = pd2_init;
            }
            break;
        case pd2_down:
            if (joystick_input < ANALOG_LOW) {
                state = pd2_down;
            } else {
                state = pd2_init;
            }
            break;
        default:
            state = pd2_init;
            break;
    }

    switch (state) {
        case pd2_init:
            break;
        case pd2_up:
            if (right_paddle.top < TOP) {
                // if we're not at the top, move right_paddle up a row
                right_paddle = moveUp(right_paddle);
                pd2 = right_paddle.top;
            }
            break;
        case pd2_down:
            if (right_paddle.bottom > BOTTOM) {
                // if we're not at the bottom, move right_paddle down a row
                right_paddle = moveDown(right_paddle);
                pd2 = right_paddle.top;
            }
            break;
        default:
            break;
    }
    return state;
}

// =============================================================================
// --------------------------- variables for ball_tick --------------------------

enum ball_states { sm_move_init,
                   sm_move_start,
                   sm_move_left,
                   sm_move_right,
                   sm_move_up_diagonal,
                   sm_move_down_diagonal,
                   sm_hit_left_paddle,
                   sm_hit_right_paddle,
                   sm_miss_paddle };

int ball_tick(int state) {
    static pos temp = {0x00, 0x00};  // a temporary variable

    // ball hits either left or right paddle
    bool hit = checkHit(curr, left_paddle) ? true : checkHit(curr, right_paddle);
    // ball has reached end if it is at LEFT or RIGHT
    bool hasReachedEnd = (curr.pattern == LEFT || curr.pattern == RIGHT);

    switch (state) {
        case sm_move_init:
            if (startGame && !endGame) {
                state = sm_move_start;
            } else {
                state = sm_move_init;
            }
            break;
        case sm_move_start:
            if (endGame) {
                state = sm_move_init;
            } else if (!endGame && !hasReachedEnd && curr.row == prev.row && curr.pattern > prev.pattern) {
                state = sm_move_left;
            } else if (!endGame && !hasReachedEnd && curr.row == prev.row && curr.pattern < prev.pattern) {
                state = sm_move_right;
            } else if (!endGame && !hasReachedEnd && curr.row > prev.row && curr.pattern != prev.pattern) {
                state = sm_move_up_diagonal;
            } else if (!endGame && !hasReachedEnd && curr.row < prev.row && curr.pattern != prev.pattern) {
                state = sm_move_down_diagonal;
            } else if (hasReachedEnd && !hit) {
                state = sm_miss_paddle;
            } else if (!endGame && hasReachedEnd && hit && curr.pattern == LEFT) {
                state = sm_hit_left_paddle;
            } else if (!endGame && hasReachedEnd && hit && curr.pattern == RIGHT) {
                state = sm_hit_right_paddle;
            } else {
                state = sm_miss_paddle;
            }
            break;
        case sm_move_left:
        case sm_move_right:
        case sm_move_up_diagonal:
        case sm_move_down_diagonal:
        case sm_hit_left_paddle:
        case sm_hit_right_paddle:
            state = sm_move_start;
            break;
        case sm_miss_paddle:
            state = sm_move_init;
            break;
        default:
            state = sm_move_init;
            break;
    }
    switch (state) {
        case sm_move_init:
            curr = updatePosition(curr, 0xFB, 0x08);
            prev = updatePosition(prev, 0xFB, 0x10);
            break;
        case sm_move_start:
            break;
        case sm_move_left:
            prev = replace(prev, curr);
            // prev = shiftPatternLeft(prev);
            curr = shiftPatternLeft(curr);
            break;
        case sm_move_right:
            prev = replace(prev, curr);
            // prev = shiftPatternRight(prev);
            curr = shiftPatternRight(curr);
            break;
        case sm_move_up_diagonal:
            temp = replace(temp, curr);
            // if diagonal up from right to left, continue to left | else continue to right
            curr = (curr.pattern > prev.pattern) ? shiftPatternLeft(curr)
                                                 : shiftPatternRight(curr);
            // if we hit top, go down
            curr = (curr.row == TOP) ? rowDown(curr) : rowUp(curr);
            prev = replace(prev, temp);
            break;
        case sm_move_down_diagonal:
            temp = replace(temp, curr);
            // if diagonal down from right to left, continue to left | else continue to right
            curr = (curr.pattern > prev.pattern) ? shiftPatternLeft(curr)
                                                 : shiftPatternRight(curr);
            // if we hit bottom, go up
            curr = (curr.row == BOTTOM) ? rowUp(curr) : rowDown(curr);
            prev = replace(prev, temp);
            break;
        case sm_hit_left_paddle:
            if (curr.row == left_paddle.top) {  // hit left paddle top, go diagonal
                prev = replace(prev, curr);
                curr = shiftPatternRight(curr);
                curr = rowDown(curr);
            } else if (curr.row == left_paddle.middle) {
                // ball has reached end and hit the left paddle, bounce back in the same row
                prev = replace(prev, curr);
                curr = shiftPatternRight(curr);
            } else if (curr.row == left_paddle.bottom) {  // ball reached end, hit left paddle's bottom row,
                prev = replace(prev, curr);
                curr = shiftPatternRight(curr);
                curr = rowUp(curr);
            }
            break;
        case sm_hit_right_paddle:
            // ball has reached end and hit the right paddle, bounce back in the same row
            if (curr.row == right_paddle.top) {
                prev = replace(prev, curr);
                curr = shiftPatternLeft(curr);
                curr = rowDown(curr);
            } else if (curr.row == right_paddle.middle) {
                // ball has reached end and hit the left paddle, bounce back in the same row
                prev = replace(prev, curr);
                curr = shiftPatternLeft(curr);
            } else if (curr.row == right_paddle.bottom) {  // ball reached end, hit left paddle's bottom row,
                prev = replace(prev, curr);
                curr = shiftPatternLeft(curr);
                curr = rowUp(curr);
            }
            break;
        case sm_miss_paddle:
            if (curr.pattern == LEFT) {
                // ball has reached left end but missed paddle, right player wins
                player1Win = true;
                player1Score++;
            } else {
                player2Win = true;
                player2Score++;
            }
            endGame = true;
            startGame = false;
            break;
        default:
            break;
    }

    return state;
}

// =============================================================================
// ------------------------ variables for  display_tick ------------------------

// display left paddle, right paddle and ball movement.

enum display_states { dis_init,
                      dis_paddle1,
                      dis_paddle2,
                      dis_ball };

int display_tick(int state) {
    static pos display = {0xFD, 0x80};

    switch (state) {
        case dis_init:  // prepare to draw the left paddle
            state = dis_paddle1;
            display = updatePosition(display, pd1, LEFT);
            break;
        case dis_paddle1:  // prepare to draw the right paddle
            state = dis_paddle2;
            display = updatePosition(display, pd2, RIGHT);  // load right paddle's values to display
            break;
        case dis_paddle2:  // prepare to draw the ball
            state = dis_ball;
            display = replace(display, curr);  // load ball's values to display
            break;
        case dis_ball:  // prepare to draw the left paddle
            state = dis_paddle1;
            display = updatePosition(display, pd1, LEFT);
        default:
            state = dis_init;
            break;
    }
    // Actions
    switch (state) {
        case dis_init:
            break;
        case dis_paddle1:
            // if we are at left paddle's bottom row, reset display to left paddles' top row
            if (display.row == left_paddle.bottom) {
                display.row = left_paddle.top;
            } else {  // move LED to the next row
                display = rowDown(display);
            }
            pd1 = display.row;  // save left paddle's row
            break;
        case dis_paddle2:
            // if we are at right paddle's bottom row, reset display to right paddles' top row
            if (display.row == right_paddle.bottom) {
                display.row = right_paddle.top;
            } else {  // move LED to the next row
                display = rowDown(display);
            }
            pd2 = display.row;
            break;
        case dis_ball:
            display = replace(display, curr);
            break;
        default:
            break;
    }
    PORTD = display.row;
    PORTC = display.pattern;

    return state;
}

// =============================================================================
// -------------------------- variables for score_tick --------------------------

enum score_states { light_init,
                    light_on };

int score_tick(int state) {
    static unsigned char output = 0x00;
    switch (state) {
        case light_init:
            state = light_on;
            break;
        case light_on:
            if ((player1Score + player1Score) >= 3) {
                state = light_init;
            }
            break;
        default:
            state = light_init;
            break;
    }
    switch (state) {
        case light_init:
            player1Win = player2Win = false;
            break;
        case light_on:
            output = player1Score | (player2Score << 3);
            break;
        default:
            state = light_init;
            break;
    }

    PORTB = output;
    return state;
}

// =============================================================================
// -------------------------- variables for reset_tick --------------------------

enum reset_states { reset_init,
                    reset_start };

int reset_tick(int state) {
    unsigned char A5 = (~PINA & 0x20);  // when A5 is pressed, reset score and matrix
    switch (state) {
        case reset_init:
            if (A5) {
                state = reset_start;
            }
            break;
        case reset_start:
            if (!A5) {
                state = reset_init;
            }
            break;
        default:
            state = reset_init;
            break;
    }
    switch (state) {
        case reset_init:
            break;
        case reset_start:
            reset();
            startGame = true;
            if (player1Score + player2Score == 3) {
                player2Score = player1Score = 0x00;
            }
            break;
        default:
            break;
    }
    return state;
}

// =============================================================================
// ================================== main ==================================
// =============================================================================

int main(void) {
    srand(time(0));

    DDRA = 0x00;
    PORTA = 0xFF;  // config PA[7:0] as inputs, init to 1's
    DDRB = 0xFF;
    PORTB = 0x00;  // config PA[7:0] as output
    DDRC = 0xFF;
    PORTC = 0x00;  // config PC[7:0] as outputs,
    DDRD = 0xFF;
    PORTD = 0x00;  // config PD[7:0] as outputs,

    static task task1, task2, task3, task4, task5, task6;
    task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6};
    const unsigned short numTasks = sizeof(tasks) / sizeof(task *);

    const char start = -1;

    // Task 1 ai_paddle_tick
    task1.state = start;
    task1.period = 200;
    task1.elapsedTime = task1.period;
    task1.TickFct = &ai_paddle_tick;

    // Task 2 paddle2_tick
    task2.state = start;
    task2.period = 200;
    task2.elapsedTime = task2.period;
    task2.TickFct = &paddle2_tick;

    // Task 3 ball_tick
    task3.state = start;
    task3.period = 100;
    task3.elapsedTime = task3.period;
    task3.TickFct = &ball_tick;

    // Task 4 display_tick
    task4.state = start;
    task4.period = 1;
    task4.elapsedTime = task4.period;
    task4.TickFct = &display_tick;

    // Task 5 score_tick
    task5.state = start;
    task5.period = 50;
    task5.elapsedTime = task4.period;
    task5.TickFct = &score_tick;

    // Task 6 reset_tick
    task6.state = start;
    task6.period = 200;
    task6.elapsedTime = task4.period;
    task6.TickFct = &reset_tick;

    unsigned int i = 0x00;
    unsigned long GCD = tasks[0]->period;

    for (i = 1; i < numTasks; i++) {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();

    // calling analog to digital conversion for joystick input reading
    A2D_init();
    Set_A2D_Pin(0x02);  // set input from joystick at port A2

    while (1) {
        for (i = 0; i < numTasks; i++) {
            if (tasks[i]->elapsedTime == tasks[i]->period) {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += GCD;
        }

        while (!TimerFlag)
            ;
        TimerFlag = 0;
    }
    return 0;  // Error: Program should not exit!
}
