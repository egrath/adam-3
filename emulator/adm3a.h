#ifndef ADM3A_H
#define ADM3A_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TERM_ROWS       24
#define TERM_COLUMNS    80

#define ASCII_NUL       0x00
#define ASCII_SOH       0x01        /* CTRL-A; start of heading */
#define ASCII_STX       0x02        /* CTRL-B; start of text */
#define ASCII_ETX       0x03        /* CTRL-C; end of text */
#define ASCII_EOT       0x04        /* CTRL-D; end of transmission */
#define ASCII_ENQ       0x05        /* CTRL-E; enquiry */
#define ASCII_ACK       0x06        /* CTRL-F; acknowledge */
#define ASCII_BEL       0x07        /* CTRL-G; bell */
#define ASCII_BS        0x08        /* CTRL-H; backspace */
#define ASCII_HT        0x09        /* CTRL-I; horizontal tab */
#define ASCII_LF        0x0A        /* CTRL-J; newline, line-feed */
#define ASCII_VT        0x0B        /* CTRL-K; vertical tab */
#define ASCII_FF        0x0C        /* CTRL-L; form feed, new-page */
#define ASCII_CR        0x0D        /* CTRL-M; carriage return */
#define ASCII_SO        0x0E        /* CTRL-N; shift-out */
#define ASCII_SI        0x0F        /* CTRL-O; shift-in */
#define ASCII_DLE       0x10        /* CTRL-P; Data link escape */
#define ASCII_DC1       0x11        /* CTRL-Q; Data control 1 */
#define ASCII_DC2       0x12        /* CTRL-R; Data control 2 */
#define ASCII_DC3       0x13        /* CTRL-S; Data control 3 */
#define ASCII_DC4       0x14        /* CTRL-T; Data control 4 */
#define ASCII_NAK       0x15        /* CTRL-U; Negative acknowledge */
#define ASCII_SYN       0x16        /* CTRL-V; Synchronous idle */
#define ASCII_ETB       0x17        /* CTRL-W; End of transmission block */
#define ASCII_CAN       0x18        /* CTRL-X; Cancel */
#define ASCII_EM        0x19        /* CTRL-Y; End of medium */
#define ASCII_SUB       0x1A        /* CTRL-Z; Substitute */
#define ASCII_ESC       0x1B        /* CTRL-[ and ESC; Escape */
#define ASCII_FS        0x1C        /* CTRL-\; File separator */
#define ASCII_GS        0x1D        /* CTRL-]; Group separator */
#define ASCII_RS        0x1E        /* CTRL-^; Record separator */
#define ASCII_US        0x1F        /* CTRL-_; Unit separator */
#define ASCII_SPACE     0x20
#define ASCII_TILDE     0x7E
#define ASCII_DEL       0x7F

#define ASCII_7         0x37        /* Symbol '7' */
#define ASCII_O         0x70        /* Symbol 'O' */

enum DecoderState
{
    DS_NORMAL,              /* we are just processing ASCII printable chars */
    DS_ESCAPE,              /* we received an ESC character, so we entered control mode and wait for a parameter */
    DS_PARAM1,              /* parameter 1 */
    DS_PARAM2,
    DS_PARAM3
};

enum CellState
{
    CS_NORMAL       = 1,
    CS_BLANK        = 2,
    CS_BLINK        = 4,
    CS_REVERSE      = 8,
    CS_UNDERLINE    = 16,
    CS_REDUCED      = 32,
    CS_GRAPHICS     = 64
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
extern Cursor cursor;

static bool adm3a_verify_input_range (int y, int x);

void adm3a_initialize (void);
void adm3a_clear_screen (void);
void adm3a_set_character (char c, int y, int x);
void adm3a_set_cursor_position (int y, int x);

void adm3a_process_character (char c);

#endif
