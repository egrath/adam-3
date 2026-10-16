/* 
 * please note that all operations work in logical coordinate mode:
 * rows: 1 .. TERM_ROWS
 * cols: 1 .. TERM_COLUMNS
 * however, the buffer is in 0-index mode!
 */
#include "adm3a.h"

Cell buffer[TERM_ROWS][TERM_COLUMNS];
Cursor cursor;

/* State of the state machine for decoding sequences */
enum DecoderState decoder_state;
uint32_t param1, param2;

/* State (normal, inverse, ...) of newly put characters */
enum CellState cell_state;

bool autonewline = true;

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

    /* initial cell state */
    cell_state = CS_NORMAL;
}

/* clear a range from the screen */
void adm3a_clear_range (Cursor from, Cursor to)
{
    int x, y;

    if (!adm3a_verify_input_range (from.y, from.x) ||
        !adm3a_verify_input_range (to.y, to.x))
        return;

    for (y = from.y-1; y < to.y; y ++)
    {
        for (x = from.x-1; x < to.x; x ++)
        {
            buffer[y][x].content = ' ';
            buffer[y][x].state = CS_BLANK;
        }
    }
}

/* clear the entire screen */
void adm3a_clear_screen (void)
{
    adm3a_clear_range (
        (Cursor) {.y=1, .x=1},
        (Cursor) {.y=TERM_ROWS, .x=TERM_COLUMNS}
    );
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
    buffer[y-1][x-1].state = cell_state;    
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
        }
    }

    /* last row needs to be cleared */
    for (col = 0; col < TERM_COLUMNS; col ++)
    {
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

        case ASCII_HT:
            /* horizontal tab */
            if (cursor.x < (TERM_COLUMNS - TABSIZE))
                cursor.x = ((cursor.x - 1) / TABSIZE + 1) * TABSIZE + 1;
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
            #ifdef DEBUG
            fprintf (stdout, "process_ascii: received ESCAPE, state is now DS_ESCAPE\n");
            #endif
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
                fprintf (stdout, "WARNING: non-handled and non-printable character received: %02X (%03d)\n", c, c);
            }
            break;
    }   

    /* clamp cursor and scroll down screen if necessary */
    if (cursor.x > TERM_COLUMNS)
    {
        if (autonewline)
        {
            cursor.x = 1;
            cursor.y ++;
        }
        else
            cursor.x = TERM_COLUMNS;
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
        /* ADM-3A Escape Sequences */
        case '(':
            #ifdef DEBUG
            fprintf (stdout, "adm3a: setting text attribute to foreground mode\n");
            #endif
            cell_state = CS_NORMAL;
            decoder_state = DS_NORMAL;
            break;

        case ')':
            #ifdef DEBUG
            fprintf (stdout, "adm3a: setting text attribute to background mode\n");
            #endif
            cell_state = CS_INVERSE;
            decoder_state = DS_NORMAL;
            break;

        case '6':
            #ifdef DEBUG
            fprintf (stdout, "not implemented: send to end of line, not implemented\n");
            #endif
            decoder_state = DS_NORMAL;
            break;

        case 'o':
            #ifdef DEBUG
            fprintf (stdout, "process_escape: lead in is 'o', state is now DS_OPER_O_P1\n");
            #endif
            decoder_state = DS_OPER_O_P1;
            break;

        case 'G':
            #ifdef DEBUG
            fprintf (stdout, "process_escape: lead in is 'G', state is now DS_OPER_G_P1\n");
            #endif
            decoder_state = DS_OPER_G_P1;
            break;

        case '=':
            #ifdef DEBUG
            fprintf (stdout, "process_escape: lead in is '=', state is now DS_OPER_EQ_P1\n");
            #endif
            decoder_state = DS_OPER_EQ_P1;
            break;

        /* now we handle some special ESC sequences which are not officially specified by the ADM-3A manual,
           but are often used in the CP/M world, even with specific ADM-3A drivers. It's because some software
           uses ADM-31 escape sequences falsly with the ADM-3A driver */
        case ASCII_ASTERISK:
        case ASCII_PLUS:
        case ASCII_SEMICOLON:
            #ifdef DEBUG
            fprintf (stdout, "adm31: clear screen, home cursor\n");
            #endif
            adm3a_clear_screen ();
            adm3a_set_cursor_position (1, 1);
            decoder_state = DS_NORMAL;
            break;

        case ASCII_UPPER_E:
            #ifdef DEBUG
            fprintf (stdout, "adm31: insert line\n");
            #endif
            adm31_insert_line ();
            decoder_state = DS_NORMAL;
            break;

        case ASCII_UPPER_R:
            #ifdef DEBUG
            fprintf (stdout, "adm31: delete line\n");
            #endif
            adm31_delete_line ();
            decoder_state = DS_NORMAL;
            break;

        case ASCII_UPPER_T:
            #ifdef DEBUG
            fprintf (stdout, "adm31: clear from cursor to end of line\n");
            #endif
            adm3a_clear_range (
                (Cursor) { .y = cursor.y, .x = cursor.x },
                (Cursor) { .y = cursor.y, .x = TERM_COLUMNS }
            );
            decoder_state = DS_NORMAL;
            break;

        case ASCII_UPPER_Y:
            #ifdef DEBUG
            fprintf (stdout, "adm31: clear from cursor to end of screen\n");
            #endif
            adm3a_clear_range (
                (Cursor) { .y = cursor.y, .x = cursor.x },
                (Cursor) { .y = cursor.y, .x = TERM_COLUMNS }
            );
            adm3a_clear_range (
                (Cursor) { .y = cursor.y+1, .x = 1 },
                (Cursor) { .y = TERM_ROWS, .x = TERM_COLUMNS }
            );
            decoder_state = DS_NORMAL;
            break;

        default:
            fprintf (stdout, "adm3a: unknown lead (%03d, [%c]) in, switching back to DS_NORMAL\n", c, c);
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

    switch (param1)
    {
        case '0':
            #ifdef DEBUG
            fprintf (stdout, "adm3a: ESC G 0, switching to normal video\n");
            #endif
            cell_state = CS_NORMAL;
            break;

        case '2':
            #ifdef DEBUG
            fprintf (stdout, "adm3a: ESC G 2, switching to blinking video\n");
            #endif
            cell_state = CS_BLINK;
            break;

        case '4':
            #ifdef DEBUG
            fprintf (stdout, "adm3a: ESC G 4, switching to reverse video\n");
            #endif
            cell_state = CS_INVERSE;
            break;

        case '6':
            #ifdef DEBUG
            fprintf (stdout, "adm3a: ESC G6, switching to reverse blinking video\n");
            #endif
            cell_state = CS_INVERSE | CS_BLINK;
            break;

        case '8':
            #ifdef DEBUG
            fprintf (stdout, "adm3a: ESC G 8, switching to underline video\n");
            #endif
            cell_state = CS_UNDERLINE;
            break;

        case ':':
            #ifdef DEBUG
            fprintf (stdout, "adm3a: ESC G :, switching to underline blinking video\n");
            #endif
            cell_state = CS_UNDERLINE | CS_BLINK;
            break;

        case '<':
            #ifdef DEBUG
            fprintf (stdout, "adm3: ESC G <, switching to underline reverse video\n");
            #endif
            cell_state = CS_INVERSE | CS_UNDERLINE;
            break;

        case '>':
            #ifdef DEBUG
            fprintf (stdout, "adm3: ESC G >, switching to underline reverse blinking video\n");
            #endif
            cell_state = CS_INVERSE | CS_UNDERLINE | CS_BLINK;
            break;

        case 'P':
            #ifdef DEBUG
            fprintf (stdout, "adm3: ESC G P, switching to graphics mode\n");
            #endif
            cell_state = CS_GRAPHICS;
            break;

        case 'R':
            #ifdef DEBUG
            fprintf (stdout, "adm3: ESC G R, switching to graphics blinky mode\n");
            #endif
            cell_state = CS_GRAPHICS | CS_BLINK;
            break;

        case 'T':
            #ifdef DEBUG
            fprintf (stdout, "adm3: ESC G T, switching to inverse graphics mode\n");
            #endif
            cell_state = CS_GRAPHICS | CS_INVERSE;
            break;

        case 'V':
            #ifdef DEBUG
            fprintf (stdout, "adm3: ESC G V, switching to inverse graphics blinky mode\n");
            #endif
            cell_state = CS_GRAPHICS | CS_INVERSE | CS_BLINK;
            break;

        default:
            fprintf (stdout, "adm3a: not implemented: operation G to set graphics attributes (param=[%c])\n", param1);
            break;
    }
}

void adm3a_process_operation_eq (void)
{
    /* here we already capture parameter 1 (row) and parameter 2 (column),
       so we can simply set the cursor */

    int row, column;

    row = param1 - ASCII_SPACE + 1;
    column = param2 - ASCII_SPACE + 1;

    #ifdef DEBUG
    fprintf (stdout, "process_operation_eq: setting cursor to row: %d column: %d\n", row, column);
    #endif

    adm3a_set_cursor_position (row, column);
}

/* process a single character in the ADM-3A emulation. These are the characters sent back
   by the host process (e.g. RunCPM) as we are working in full-duplex mode */
void adm3a_eat (char c)
{
    #ifdef DEBUG
    fprintf (stdout, "        incoming: 0x%02X (%03d, [%c]), DS=%d\n", c, c, (c >= ASCII_SPACE && c <= ASCII_DEL) ? c : ' ', decoder_state);
    #endif  
    switch (decoder_state)
    {
        case DS_NORMAL:
            adm3a_process_ascii (c);
            break;
        case DS_ESCAPE:
            #ifdef DEBUG
            fprintf (stdout, "eat: state is DS_ESCAPE, processing lead-in\n");
            #endif
            adm3a_process_escape (c);
            break;
        case DS_OPER_O_P1:
            param1 = c;
            #ifdef DEBUG
            fprintf (stdout, "eat: state is DS_OPER_O_P1, param1 = %03d (%c), processing\n", param1, param1);
            #endif
            adm3a_process_operation_o ();
            decoder_state = DS_NORMAL;
            break;
        case DS_OPER_G_P1:
            param1 = c;
            #ifdef DEBUG
            fprintf (stdout, "eat: state is DS_OPER_G_P1, param1 = %03d (%c), processing\n", param1, param1);
            #endif
            adm3a_process_operation_g ();
            decoder_state = DS_NORMAL;
            break;
        case DS_OPER_EQ_P1:
            param1 = c;
            #ifdef DEBUG
            fprintf (stdout, "eat: state is DS_OPER_O_P1, param1 is %03d, state is now DS_OPER_EQ_P2\n", param1);
            #endif
            decoder_state = DS_OPER_EQ_P2;
            break;
        case DS_OPER_EQ_P2:
            param2 = c; 
            #ifdef DEBUG
            fprintf (stdout, "eat: state is DS_OPER_O_P2, param2 is %03d, processing\n", param2);
            #endif
            adm3a_process_operation_eq ();
            decoder_state = DS_NORMAL;
            break;

        default:
            decoder_state = DS_NORMAL;
            break;
    }
}

void adm31_insert_line (void)
{
    int sr, dr, col;

    /* copy from the second to last row down */
    for (sr = TERM_ROWS-1; sr >= cursor.y; sr --)
    {
        dr = sr+1;
        for (col = 1; col <= TERM_COLUMNS; col ++)
        {
            buffer[dr-1][col-1] = buffer[sr-1][col-1];
        }
    }

    /* clear the line with the cursor */
    adm3a_clear_range(
        (Cursor) { .y = cursor.y, .x = 1 },
        (Cursor) { .y = cursor.y, .x = TERM_COLUMNS} );

    /* move cursor to proper position */
    adm3a_set_cursor_position (cursor.y, 1);
}

void adm31_delete_line (void)
{
    int sr, dr, col;

    /* copy from the second to last row down */
    for (sr = cursor.y+1; sr <= TERM_ROWS; sr++)
    {
        dr = sr-1;
        for (col = 1; col <= TERM_COLUMNS; col ++)
        {
            buffer[dr-1][col-1] = buffer[sr-1][col-1];
        }
    }

    /* clear the line with the cursor */
    adm3a_clear_range(
        (Cursor) { .y = TERM_ROWS, .x = 1 },
        (Cursor) { .y = TERM_ROWS, .x = TERM_COLUMNS} );

    /* move cursor to proper position */
    adm3a_set_cursor_position (cursor.y, 1);
}
