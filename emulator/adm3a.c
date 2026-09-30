#include "adm3a.h"

Cell buffer[TERM_ROWS][TERM_COLUMNS];
Cursor cursor;

/* initialize the terminal to it's pristine state */
void adm3a_initialize (void)
{
    int x, y;

    /* clear terminal buffer */
    for (y = 0; y < TERM_ROWS; y ++)
    {
        for (x = 0; x < TERM_COLUMNS; x ++)
        {
            buffer[y][x].content = ' ';
            buffer[y][x].state = CS_BLANK;
            buffer[y][x].x = x + 1;
            buffer[y][x].y = y + 1;
        }
    }

    /* set cursor position */
    cursor.x = cursor.y = 1;

    adm3a_set_string("Initialized", 3, 1);
}

/* verify if given coordinates are within our visible range */
static bool adm3a_verify_input_range (int y, int x)
{
    return (y >= 1 && y <= TERM_ROWS) && (x >= 1 && x <= TERM_COLUMNS);
}

/* set a single character in the terminal buffer, at absolute position */
void adm3a_set_character (char c, int y, int x)
{
    if( !adm3a_verify_input_range (y, x))
        return;

    fprintf (stdout, "setting character [%c] at position (r:%d,c:%d) [in buffer (r:%d,c:%d)]\n",
        c, y, x, y-1, x-1);

    buffer[y-1][x-1].content = c;
    buffer[y-1][x-1].x = x;
    buffer[y-1][x-1].y = y;
    buffer[y-1][x-1].state = CS_NORMAL;    
}

/* write a string into the terminal buffer at absolute position */
void adm3a_set_string (char *s, int y, int x)
{
    if (!adm3a_verify_input_range (y, x))
        return;

    while (*s != '\0')
    {
        adm3a_set_character (*s, y, x++);
        s++;
    }
}
