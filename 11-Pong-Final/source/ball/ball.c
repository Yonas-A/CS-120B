#include <stdbool.h>
#include "../header/utility.h"

static bool endGame = false;
static bool startGame = true;

bool player1Win = false;
bool player2Win = false;

static pos temp = {0x00, 0x00};
static pos curr = {0xFB, 0x08};  // holds the current position of the ball on the LED matrix
static pos prev = {0xFB, 0x10};  // holds the previous position of the ball on the LED matrix

static bool hit = false;           // records if the ball hits the paddles or misses them
static bool hasReachedEnd = true;  // records if the ball has reached paddles or not

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
    // ball hits either left or right paddle
    hit = checkHit(curr, left_paddle) ? true : checkHit(curr, right_paddle);
    // ball has reached end if it is at LEFT or RIGHT
    hasReachedEnd = (curr.pattern == LEFT || curr.pattern == RIGHT);

    switch (state) {
        case sm_move_init:
            if (startGame && !endGame) {
                state = sm_move_start;
            } else {
                state = sm_move_init;
            }
            break;
        case sm_move_start:
            if (!endGame && curr.row == prev.row && curr.pattern != LEFT && curr.pattern > prev.pattern) {
                state = sm_move_left;
            } else if (!endGame && curr.row == prev.row && curr.pattern != RIGHT && curr.pattern < prev.pattern) {
                state = sm_move_right;
            } else if (!endGame && curr.row > prev.row && curr.pattern != prev.pattern && curr.pattern != RIGHT && curr.pattern != LEFT) {
                state = sm_move_up_diagonal;
            } else if (!endGame && curr.row < prev.row && curr.pattern != prev.pattern && curr.pattern != RIGHT && curr.pattern != LEFT) {
                state = sm_move_down_diagonal;
            } else if (!endGame && hit && curr.pattern == LEFT) {
                state = sm_hit_left_paddle;
            } else if (!endGame && hit && curr.pattern == RIGHT) {
                state = sm_hit_right_paddle;
            } else if (!endGame && !hit && hasReachedEnd) {
                state = sm_miss_paddle;
            } else if (endGame == true) {
                state = sm_move_init;
            }
            break;
        case sm_move_left:
        case sm_move_right:
        case sm_move_up_diagonal:
        case sm_move_down_diagonal:
        case sm_hit_left_paddle:
        case sm_hit_right_paddle:
        case sm_miss_paddle:
            state = sm_move_start;
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
            prev = shiftPatternLeft(prev);
            curr = shiftPatternLeft(curr);
            break;
        case sm_move_right:
            prev = shiftPatternRight(prev);
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
                // left paddle is moving & paddle not at TOP put spin on the ball
                curr = ((~PINA & 0x01 || ~PINA & 0x02) && curr.row != TOP) ? rowUp(curr) : rowDown(curr);
            } else if (curr.row == left_paddle.middle) {
                // ball has reached end and hit the left paddle, bounce back in the same row
                prev = replace(prev, curr);
                curr = shiftPatternRight(curr);
            } else {  // ball reached end, hit left paddle's bottom row,
                prev = replace(prev, curr);
                curr = shiftPatternRight(curr);
                // left paddle is moving & paddle not at BOTTOM put spin on the ball
                curr = ((~PINA & 0x01 || ~PINA & 0x02) && curr.row != BOTTOM) ? rowDown(curr) : rowUp(curr);
            }
            break;
        case sm_hit_right_paddle:
            // ball has reached end and hit the right paddle, bounce back in the same row
            if (curr.row == right_paddle.top) {
                prev = replace(prev, curr);
                curr = shiftPatternLeft(curr);
                // right paddle is moving & paddle not at TOP put spin on the ball
                curr = ((ADC > 600 || ADC < 450) && curr.row != TOP) ? rowUp(curr) : rowDown(curr);
            } else if (curr.row == right_paddle.middle) {
                // ball has reached end and hit the left paddle, bounce back in the same row
                prev = replace(prev, curr);
                curr = shiftPatternLeft(curr);
            } else {  // ball reached end, hit left paddle's bottom row,
                prev = replace(prev, curr);
                curr = shiftPatternLeft(curr);
                // right paddle is moving & paddle not at BOTTOM put spin on the ball
                curr = ((ADC > 600 || ADC < 450) && curr.row != BOTTOM) ? rowDown(curr) : rowUp(curr);
            }
            break;
        case sm_miss_paddle:
            if (curr.pattern == LEFT) {
                // ball has reached left end but missed paddle, right player wins
                player1Win = true;
                PORTB = 0x01;
            } else if (curr.pattern == RIGHT) {
                // ball has reached right end but missed paddle, left player wins
                player2Win = true;
                PORTB = 0x08;
            }
            endGame = true;
            startGame = false;
            break;
        default:
            break;
    }

    return state;
}