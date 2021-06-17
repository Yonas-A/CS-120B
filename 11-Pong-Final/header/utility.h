#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOP 0xFE
#define BOTTOM 0xEF
#define LEFT 0x80
#define RIGHT 0x01
#define MIDDLECOL 0x08


/**
 * A struct that holds the positions of a three row paddle on a 5x8 LED matrix
 * @param top the position of the top row of a paddle along the row of the matrix
 * @param middle the position of the middle row of a paddle along the row of the matrix
 * @param bottom the position of the bottom row of a paddle along the row of the matrix
 */
typedef struct paddle {
    unsigned char top;     //  the position of the top row of the paddle
    unsigned char middle;  //  the position of the middle row of the paddle
    unsigned char bottom;  //  the position of the bottom row of the paddle
} paddle;

/**
 * changes A's member variables to move one row up in an LED Matrix
 * @param A a paddle struct that will updated
 */
paddle moveUp(paddle A) {
    A.bottom = A.middle;
    A.middle = A.top;
    A.top = A.top >> 1 | 0x80;
    return (A);
}

/**
 * changes A's member variables to move one row down in an LED Matrix
 * @param A a paddle struct that will updated
 */
paddle moveDown(paddle A) {
    A.top = A.middle;
    A.middle = A.bottom;
    A.bottom = A.bottom << 1 | 0x01;
    return (A);
}

/**
 * changes A's member variables to move one row down in an LED Matrix
 * @param A a paddle struct that will updated
 */
paddle updatePaddle(paddle A, unsigned char x, unsigned char y, unsigned char z) {
    A.top = x;
    A.middle = y;
    A.bottom = z;
    return (A);
}

// ============================================================================
// ============================================================================
// ----------- struct for the position of an LED light on LED Matrix ----------
// ============================================================================
// ============================================================================

/**
 * a row and pattern definition for a 5x8 LED matrix
 * @param row the row position of an object in the led matrix
 * @param pattern the pattern / column position of a single light in the led matrix
 */
typedef struct position {
    unsigned char row;      // the row of an led matrix
    unsigned char pattern;  // the pattern of an led matrix

} pos;

/**
 * changes the row and pattern value of A to the value of the passed struct
 * @param A a position struct that will be updated
 * @param B a position struct that will that will replace A's values
 */
pos replace(pos A, pos B) {
    A.row = B.row;
    A.pattern = B.pattern;
    return (A);
}

/**
 * changes the row and pattern value of A to the value of the passed values
 * @param A a position struct that will be updated
 * @param row a char value that will be used to update A's row value
 * @param pattern a char value that will be used to update A's pattern value
 */
pos updatePosition(pos A, unsigned char row, unsigned char pattern) {
    A.row = row;
    A.pattern = pattern;
    return (A);
}

/**
 * sets the row value of A to the value of the passed char
 * @param A a position struct
 * @param pattern a value that will replace A.row's value
 */
pos updateRow(pos A, unsigned char row) {
    A.row = row;
    return (A);
}

/**
 * sets the pattern value of A to the value of the passed char
 * @param A a position struct
 * @param pattern a value that will replace A.pattern's value
 */
pos updatePattern(pos A, unsigned char pattern) {
    A.pattern = pattern;
    return (A);
}

/**
 * moves the row value of A one value down a row
 * @param A a position struct that will be moved down by one row
 */
pos rowDown(pos A) {
    A.row = (A.row << 1) | 0x01;
    return (A);
}

/**
 * moves the row value of A one value up a row
 * @param A a position struct that will be moved up by one row
 */
pos rowUp(pos A) {
    A.row = (A.row >> 1) | 0x80;
    return (A);
}

/**
 * moves A's pattern one value to the left
 * @param A a position struct that will be shifted left by one pattern/column
 */
pos shiftPatternLeft(pos A) {
    A.pattern = (A.pattern << 1);
    return (A);
}

/**
 * moves the pattern value of A one value to the right
 * @param A a position struct that will be shifted right by one pattern/column
 */
pos shiftPatternRight(pos A) {
    A.pattern = (A.pattern >> 1);
    return (A);
}

// ============================================================================
// ============================================================================
// ---------------------------- helper functions ----------------------------
// ============================================================================
// ============================================================================

/**
 * checks if ball hits the paddle
 * @param A a position struct that will be used as the ball's current position
 * @param p a paddle struct that will be used as the paddle's current position
 */
bool checkHit(pos A, paddle p) {
    if (A.pattern == LEFT || A.pattern == RIGHT) {
        return ((A.row == p.top) || (A.row == p.middle) || (A.row == p.bottom));
    }
    return false;
}

bool hasHit(pos A, unsigned char side, paddle p) {
    if (A.pattern == side) {
        return ((A.row == p.top) || (A.row == p.middle) || (A.row == p.bottom));
    }
    return false;
}

/**
 * Returns a random number in the range between @code low and @code high
 * @param low the highest number the function can return
 * @param high the highest number the function can return
 */
unsigned int next(unsigned char low, unsigned char high) {
    return (rand() % (high - low + 1) + low);
}

#endif  // __UTILITY_H__