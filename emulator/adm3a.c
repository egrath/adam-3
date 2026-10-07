#include "adm3a.h"

Cell buffer[TERM_ROWS][TERM_COLUMNS];
Cursor cursor;

enum DecoderState decoder_state;
char param1, param2, param3;

/* used for temporarily storing received control parameters from the host */
char parameter1, parameter2;

/* initialize the terminal to it's pristine state */
void adm3a_initialize (void)
{
    adm3a_clear_screen ();

    /* set cursor position */
    cursor.x = cursor.y = 1;

    /* initial decoder state */
    decoder_state = DS_NORMAL;
}

/* clear the entire screen */
void adm3a_clear_screen (void)
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

    #ifdef DEBUG
    /*
    fprintf (stdout, "DEBUG: ADM-3A: setting character [%c] at position (r:%d,c:%d) [in buffer (r:%d,c:%d)]\n",
        c, y, x, y-1, x-1);
    fflush (stdout);
    */
    #endif

    buffer[y-1][x-1].content = c;
    buffer[y-1][x-1].x = x;
    buffer[y-1][x-1].y = y;
    buffer[y-1][x-1].state = CS_NORMAL;    
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

/* process a single character in the ADM-3A emulation. These are the characters sent back
   by the host process (e.g. RunCPM) as we are working in full-duplex mode */
void adm3a_process_character (char c)
{
    fprintf (stdout, "    inc: 0x%02X (%03d, [%c])\n", c, c, (c >= ASCII_SPACE && c <= ASCII_DEL) ? c : ' ');

    if (decoder_state == DS_NORMAL)
    {
        switch (c)
        {
            case ASCII_BEL:
                /* sounds a audible tone in the terminal */
                fprintf (stdout, "Ding Dong!\n");
                break;

            case ASCII_BS:
                /* move cursor on column to the left */
                if (cursor.x > 1)
                    cursor.x--;
                break;

            case ASCII_LF:
                /* move cursor down one row and remain in the same column */
                cursor.y ++;
                break;

            case ASCII_VT:
                /* move cursor up on row and remain in the same column */
                if (cursor.y > 1)
                    cursor.y --;
                break;

            case ASCII_FF:
                /* move cursor one column to the right */
                if (cursor.x < TERM_COLUMNS)
                    cursor.x ++;
                break;

            case ASCII_CR:
                /* move cursor to the first column of the current row */
                cursor.x = 1;
                break;

            case ASCII_SO:
                /* not implemented */
                break;

            case ASCII_SI:
                /* not implemented */
                break;

            case ASCII_SUB:
                /* clear screen */
                adm3a_clear_screen ();
                adm3a_set_cursor_position (1, 1);
                break;

            case ASCII_ESC:
                /* Command lead-in */
                decoder_state = DS_PARAM1;
                fprintf (stdout, "new state is ds_param1\n");
                break;

            case ASCII_RS:
                /* Move cursor to home position */
                adm3a_set_cursor_position (1, 1);
                break;

            default:
                /* we only set printable characters */
                if (c >= ASCII_SPACE && c <= ASCII_TILDE)
                {
                    adm3a_set_character (c, cursor.y, cursor.x);
                    cursor.x ++;
                }
                else
                {
                    fprintf (stdout, "non-printable character received: %02X (%03d)\n", c, c);
                }
                break;
        }
    }
    else if (decoder_state == DS_PARAM1)
    {
        param1 = c;
        if (param1 == '=' || param1 == 'G')
            decoder_state = DS_PARAM2;
        else
            decoder_state = DS_NORMAL;
    }
    else if (decoder_state == DS_PARAM2)
    {
        param2 = c;
        /* are we processing a cursor set? */
        if (param1 == '=' && param2 >= ASCII_SPACE && param2 <= ASCII_7)
            decoder_state = DS_PARAM3;
        else if (param1 == 'G')
        {
            /* we are processing a set video attribute */
            fprintf (stdout, "set video attribute = [%c]\n", param2);
            decoder_state = DS_NORMAL;
        }
        else
            decoder_state = DS_NORMAL;
    }
    else if (decoder_state == DS_PARAM3)
    {
        param3 = c;
        if (param3 >= ASCII_SPACE && param3 <= ASCII_O)
        {
            int row = param2 - ASCII_SPACE + 1;
            int col = param3 - ASCII_SPACE + 1;
            adm3a_set_cursor_position (row, col);
        }
        
        decoder_state = DS_NORMAL;
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
