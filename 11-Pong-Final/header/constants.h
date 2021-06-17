#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// ========================================================================
// a 5x8 LED matrix pattern to display '0' on the left side of the LED
unsigned const char ZERO_ROW[] = {0xFE, 0xFE, 0xFE, 0xFD, 0xFB, 0xF7, 0xEF,
                                  0xEF, 0xEF, 0xF7, 0xFB, 0xFD};
unsigned const char ZERO_PATTERN[] = {0x80, 0x40, 0x20, 0x20, 0x20, 0x20, 0x20,
                                      0x40, 0x80, 0x80, 0x80, 0x80};
unsigned char ZERO_LENGTH = sizeof(ZERO_ROW) / sizeof(ZERO_ROW[0]);

// ========================================================================
// a 5x8 LED matrix pattern to display '1' on the left side of the LED
unsigned const char ONE_ROW[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF};
unsigned const char ONE_PATTERN[] = {0x40, 0x40, 0x40, 0x40, 0x40};
unsigned const char ONE_LENGTH = sizeof(ONE_ROW) / sizeof(ONE_ROW[0]);

// ========================================================================
// a 5x8 LED matrix pattern to display '2' on the left side of the LED
unsigned const char TWO_ROW[] = {0xFE, 0xFE, 0xFE, 0xFD, 0xFB, 0xFB, 0xFB, 0xF7,
                                 0xEF, 0xEF, 0xEF};
unsigned const char TWO_PATTERN[] = {0x80, 0x40, 0x20, 0x20, 0x20, 0x40, 0x80,
                                     0x80, 0x80, 0x40, 0x20};
unsigned const char TWO_LENGTH = sizeof(TWO_ROW) / sizeof(TWO_ROW[0]);

// ========================================================================
// a 5x8 LED matrix pattern to display '3'  on the left side of the LED
unsigned const char THREE_ROW[] = {0xFE, 0xFE, 0xFE, 0xFD, 0xFB, 0xFB, 0xFB,
                                   0xFB, 0xFB, 0xF7, 0xEF, 0xEF, 0xEF};
unsigned const char THREE_PATTERN[] = {0x80, 0x40, 0x20, 0x20, 0x20, 0x40, 0x80,
                                       0x40, 0x20, 0x20, 0x20, 0x40, 0x80};
unsigned const char THREE_LENGTH = sizeof(THREE_ROW) / sizeof(THREE_ROW[0]);

// ========================================================================
// a 5x8 LED matrix pattern to display 'E'  on the left side of the LED
unsigned const char E_ROW[] = {0xFE, 0xFE, 0xFD, 0xFB, 0xFB, 0xFB, 0xF7, 0xEF,
                               0xEF};
unsigned const char E_PATTERN[] = {0x40, 0x80, 0x80, 0x80, 0x40, 0x80, 0x80,
                                   0x80, 0x40};
unsigned const char E_LENGTH = sizeof(E_ROW) / sizeof(E_ROW[0]);

// ========================================================================
// a 5x8 LED matrix pattern to display 'N'  on the left side of the LED
unsigned const char N_ROW[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xFB, 0xEF, 0xF7,
                               0xFB, 0xFD, 0xFE};
unsigned const char N_PATTERN[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x08,
                                   0x08, 0x08, 0x08, 0x08};
unsigned const char N_LENGTH = sizeof(N_ROW) / sizeof(N_ROW[0]);

// ========================================================================
// a 5x8 LED matrix pattern to display 'D'  on the left side of the LED
unsigned const char D_ROW[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xFE, 0xFD, 0xFB,
                               0xF7, 0xEF};
unsigned const char D_PATTERN[] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x02, 0x01,
                                   0x01, 0x01, 0x02};
unsigned const char D_LENGTH = sizeof(D_ROW) / sizeof(D_ROW[0]);

#endif  // __CONSTANTS_H__