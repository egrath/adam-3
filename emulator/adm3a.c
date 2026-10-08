#include "adm3a.h"

Cell buffer[TERM_ROWS][TERM_COLUMNS];
Cursor cursor;

enum DecoderState decoder_state;
uint32_t param1, param2;

/* used for the terminal bell */
void (*adm3a_bell) (void) = NULL;

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

void adm3a_process_ascii (uint32_t c)
{
    switch (c)
    {
        case ASCII_BEL:
            /* produces some kind of terminal bell */
            if (adm3a_bell)
                adm3a_bell ();
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
            fprintf (stdout, "process_ascii: received ESCAPE, state is now DS_ESCAPE\n");
            decoder_state = DS_ESCAPE;
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

    /* clamp cursor and scroll down screen if necessary */
    if (cursor.x > TERM_COLUMNS)
    {
        cursor.x = 1;
        cursor.y ++;
    }

    if (cursor.x < 1)
        cursor.x = 1;

    if (cursor.y > TERM_ROWS)
    {
        adm3a_scroll_down ();
        cursor.y = TERM_ROWS;
    }

    if (cursor.y < 1)
        cursor.y = 1;
}

void adm3a_process_escape (uint32_t c)
{
    switch (c)
    {
        case '(':
            fprintf (stdout, "not implemented: set foreground mode\n");
            decoder_state = DS_NORMAL;
            break;

        case ')':
            fprintf (stdout, "not implemented: set background mode\n");
            decoder_state = DS_NORMAL;
            break;

        case '6':
            fprintf (stdout, "not implemented: send to end of line, not implemented\n");
            decoder_state = DS_NORMAL;
            break;

        case 'o':
            fprintf (stdout, "process_escape: lead in is 'o', state is now DS_OPER_O_P1\n");
            decoder_state = DS_OPER_O_P1;
            break;

        case 'G':
            fprintf (stdout, "process_escape: lead in is 'G', state is now DS_OPER_G_P1\n");
            decoder_state = DS_OPER_G_P1;
            break;

        case '=':
            fprintf (stdout, "process_escape: lead in is '=', state is now DS_OPER_EQ_P1\n");
            decoder_state = DS_OPER_EQ_P1;
            break;

        default:
            fprintf (stdout, "process_escape: unknown lead in, switching back to DS_NORMAL\n");
            decoder_state = DS_NORMAL;
            break;
    }
}

void adm3a_process_operation_o (void)
{
    /* param1 contains the operation to perform */

    if (param1 == ASCII_EXCLAM) /* terminal reset */
        adm3a_initialize ();
    else if (param1 == ASCII_UPPER_S) /* set terminal to default conditions */
        fprintf (stdout, "not implemented: reset terminal to default state\n");
    else if (param1 == ASCII_9) /* send version information to host */
        fprintf (stdout, "not implemented: send version information to host\n");
}

void adm3a_process_operation_g (void)
{
    /* param1 contains the graphics attribute we have to set */

    fprintf (stdout, "not implemented: operation G to set graphics attributes (param=[%c])\n", param1);
}

void adm3a_process_operation_eq (void)
{
    /* here we already capture parameter 1 (row) and parameter 2 (column),
       so we can simply set the cursor */

    int row, column;

    row = param1 - ASCII_SPACE + 1;
    column = param2 - ASCII_SPACE + 1;

    fprintf (stdout, "process_operation_eq: setting cursor to row: %d column: %d\n", row, column);

    adm3a_set_cursor_position (row, column);
}

/* process a single character in the ADM-3A emulation. These are the characters sent back
   by the host process (e.g. RunCPM) as we are working in full-duplex mode */
void adm3a_eat (char c)
{
    // fprintf (stdout, "    inc: 0x%02X (%03d, [%c])\n", c, c, (c >= ASCII_SPACE && c <= ASCII_DEL) ? c : ' ');
    switch (decoder_state)
    {
        case DS_NORMAL:
            adm3a_process_ascii (c);
            break;
        case DS_ESCAPE:
            fprintf (stdout, "eat: state is DS_ESCAPE, processing lead-in\n");
            adm3a_process_escape (c);
            break;
        case DS_OPER_O_P1:
            param1 = c;
            fprintf (stdout, "eat: state is DS_OPER_O_P1, param1 = %03d (%c), processing\n", param1, param1);
            adm3a_process_operation_o ();
            decoder_state = DS_NORMAL;
            break;
        case DS_OPER_G_P1:
            param1 = c;
            fprintf (stdout, "eat: state is DS_OPER_G_P1, param1 = %03d (%c), processing\n", param1, param1);
            adm3a_process_operation_g ();
            decoder_state = DS_NORMAL;
            break;
        case DS_OPER_EQ_P1:
            param1 = c;
            fprintf (stdout, "eat: state is DS_OPER_O_P1, param1 is %03d, state is now DS_OPER_EQ_P2\n", param1);
            decoder_state = DS_OPER_EQ_P2;
            break;
        case DS_OPER_EQ_P2:
            param2 = c; 
            fprintf (stdout, "eat: state is DS_OPER_O_P2, param2 is %03d, processing\n", param2);
            adm3a_process_operation_eq ();
            decoder_state = DS_NORMAL;
            break;

        default:
            decoder_state = DS_NORMAL;
            break;
    }
}
