#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <termios.h>
#include <unistd.h>

struct termios old_termios;

/* enter raw mode for the terminal.
 * references:
 *     - Kilo Editor (https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html)
 *     - The Linux Programming Interface book */   
void enableRawMode (void)
{
    struct termios raw;

    /* save old (current) terminal settings */
    tcgetattr (STDIN_FILENO, &old_termios);

    tcgetattr (STDIN_FILENO, &raw);

    /* disable local echo, canonical input processing and the interrupt signal (Ctrl-C, Ctrl-Z) */
    /* disable extended input processing (IEXTEN) for Ctrl-V */
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN );

    /* disable software flow control (Ctrl-S, Ctrl-Q) */
    /* disable newline processing (ICRNL) for Ctrl-M and ENTER key in the input */
    raw.c_iflag &= ~(IXON | ICRNL);

    /* disable newline processing for the output, so when we send a '\n', really
       just send a '\n' and not a '\r\n' */
    raw.c_oflag &= ~(OPOST);

    /* also set some other flags which should not matter, but just to make sure */
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP );
    raw.c_cflag |= ~(CS8);

    tcsetattr (STDIN_FILENO, TCSANOW, &raw);
}

void disableRawMode (void)
{
    tcsetattr (STDIN_FILENO, TCSANOW, &old_termios);
}

int main (int argc, char **argv)
{
    int character = 0, count = 0;

    enableRawMode();

    while (character != 0x20) /* stop when space is received */
    {
        character = fgetc (stdin);
        
        fprintf (stdout, "%02X", character);
        if ((++count % 20) == 0)
            fprintf (stdout,"\r\n");
        else
            fprintf (stdout, " ");
    }

    disableRawMode();

    return 0;
}
