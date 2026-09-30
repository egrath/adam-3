#ifndef ADM3A_H
#define ADM3A_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TERM_ROWS       25
#define TERM_COLUMNS    80

enum CellState
{
    CS_NORMAL,
    CS_BLANK,
    CS_BLINK,
    CS_REVERSE,
    CS_UNDERLINE,
    CS_REDUCED,
    CS_GRAPHICS
};

typedef struct
{
    int x, y;
    enum CellState state;
    char content;
}
Cell;

typedef struct
{
    int x, y;
}
Cursor;

extern Cell buffer[TERM_ROWS][TERM_COLUMNS];

static bool adm3a_verify_input_range (int y, int x);

void adm3a_initialize (void);
void adm3a_set_character (char c, int y, int x);
void adm3a_set_string (char *s, int y, int x);

#endif
