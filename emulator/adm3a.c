#include "adm3a.h"

Cell buffer[TERM_ROWS][TERM_COLUMNS];
Cursor cursor;
enum DecoderState decoder_state;

/* used for temporarily storing received control parameters from the host */
char parameter1, parameter2;

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

/* set the cursor to a new position */
void adm3a_set_cursor_position (int y, int x)
{
    if (!adm3a_verify_input_range (y, x))
        return;

    cursor.y = y;
    cursor.x = x;
}

/* scroll down the terminal screen by one line */
void adm3a_scroll_down (void)
{
    int row, col;

    /* we are deliberately not using memcpy to poke around with the array, because
       i want to have it readable */
    for (row = 1; row < TERM_ROWS; row ++)
    {
        for (col = 0; col < TERM_COLUMNS; col ++)
        {
            buffer[row-1][col] = buffer[row][col];
            buffer[row-1][col].y--;
        }
    }

    /* last row needs to be cleared */
    for (col = 0; col < TERM_COLUMNS; col ++)
    {
        buffer[TERM_ROWS-1][col].y = TERM_ROWS;
        buffer[TERM_ROWS-1][col].x = col+1;
        buffer[TERM_ROWS-1][col].state = CS_BLANK;
        buffer[TERM_ROWS-1][col].content = ' '; 
    }
}

/* put a character at the current cursor position and:
   - advance the cursor and switch to the next line if necessary
   - scroll the terminal if already at the end */
void adm3a_put_character (char c)
{
    switch (c)
    {
        case ASCII_LF:
            cursor.y ++;
            break;

        case ASCII_CR:
            cursor.x = 1;
            break;

        default:
            adm3a_set_character (c, cursor.y, cursor.x);
            cursor.x ++;
            break;
    }

    /* move cursor and screen if necessary */
    if (cursor.x > TERM_COLUMNS)
    {
        cursor.x = 1;
        cursor.y ++;
    }

    if (cursor.y > TERM_ROWS)
    {
        adm3a_scroll_down ();
        cursor.y = TERM_ROWS;
    }
}

/* this is essentially our most important function in the emulation - it receives
   data from the host and updates internal states (text content, cursor ...) */
void adm3a_receive (char c)
{
    /* are we already in a escape sequence? */
    if (decoder_state == DS_ESCAPE)
    {
        switch (c)
        {
            case '(':
                fprintf (stderr, "adm3a: set foreground mode not yet implemented\n");
                break;

            case ')':
                fprintf (stderr, "adm3a: set background mode not yet implemented\n");
                break;

            case 'G':
                decoder_state = DS_SET_VIDEO_ATTRIBUTE;
                break;

            case 'o':
                decoder_state = DS_TERM_OPER;
                break;

            default:
                /* receive row for set cursor and set to 1 or 24 if outside of valid range */
                if (c >= ASCII_SPACE && c <= '7')
                    parameter1 = c - ASCII_SPACE + 1;
                else if (c < ASCII_SPACE)
                    parameter1 = 1;
                else if (c > '7')
                    parameter1 = TERM_ROWS;
                decoder_state = DS_SET_CURSOR_PARAM2;
                break;
        }
    }
    else if (decoder_state == DS_SET_CURSOR_PARAM2)
    {
        /* we are about to receive the second parameter for the set cursor command (column) */
        if (c >= ASCII_SPACE && c <= 'o')
            parameter2 = c - ASCII_SPACE + 1;
        else if (c < ASCII_SPACE)
            parameter2 = 1;
        else if (c > 'o')
            parameter2 = TERM_COLUMNS;

        fprintf (stdout, "adm3: set cursor command received, new position: row=%d, column=%d\n",
            parameter1, parameter2);

        adm3a_set_cursor_position (parameter1, parameter2);

        decoder_state = DS_NORMAL;
    }
    else
    {
        if (c == ASCII_ESCAPE)
            decoder_state = DS_ESCAPE;
        else
        {
            fprintf (stdout, "adm3: received ascii [%c] (%02X, %03d)\n", c, c, c);
            adm3a_put_character (c);
        }
    }
}
