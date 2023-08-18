#include "print.h"
#include <stdio.h>

const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;

struct Char {
    uint8_t character;
    uint8_t color;
};

struct Char* buffer = (struct Char*) 0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = WHITE | BLACK << 4;

void clear_row(size_t row) {
    struct Char empty = (struct Char) {
        character: ' ',
        color: color,
    };  

    for (size_t col = 0; col < NUM_COLS; col++) {
        buffer[col + NUM_COLS * row] = empty;
    }
}

void go_backspace() {
    if (col > 0) { // check if the cursor shit is not at the start of a row
        col--;
        
        // fill that bitch in w a empty Char type
        buffer[col + NUM_COLS * row] = (struct Char) {
            .character = ' ',
            .color = color,
        };
    }
}

void clear() {
    for (size_t i = 0; i < NUM_ROWS; i++) {
        clear_row(i);
    }
    col = 0;
    row = 0; // Reset the cursor to the first row
}

void print_newline() {
    col = 0;

    if (row < NUM_ROWS - 1) { // Normal function, if there is room left.
        row++;
        return;
    }
    
    clear(); return;

    // Used for if you want the page to keep "scrolling down".
    // VERY BUGGY
    //
    // for (size_t row = 1; row < NUM_ROWS; row++) {
    //     for (size_t col = 0; col < NUM_COLS; col++) {
    //         struct Char character = buffer[col + NUM_COLS * row];
    //         buffer[col + NUM_COLS * (row - 1)] = character;
    //     }
    // }

    // clear_row(NUM_COLS - 1);
}

void print_char(char character) {
    if (character == '\n') {
        print_newline();
        return;
    }

    if (col > NUM_COLS) {
        print_newline();
    }

    buffer[col + NUM_COLS * row] = (struct Char) {
        character: (uint8_t) character,
        color: color,
    };

    col++;
}

void print(char* str) {
    for (size_t i = 0; 1; i++) {
        char character = (uint8_t) str[i];

        if (character == '\0') {
            return;
        }

        print_char(character);
    }
}


void cprint(char* string, uint8_t fg, uint8_t bg) {
    color = fg + (bg << 4);
    print(string); 
    color = WHITE + (BLACK << 4);
}
