/*
 * PTY test program slave
 * dumps every input received on STDIN as hex codes to STDOUT
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <windows.h>

DWORD old_input_mode;
DWORD old_output_mode;

void enable_raw_mode (void)
{
    DWORD input_flags = ~(ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    DWORD output_flags = ~(ENABLE_PROCESSED_OUTPUT);

    if (GetConsoleMode (GetStdHandle (STD_INPUT_HANDLE), &old_input_mode) == 0)
    {
        fprintf (stdout, "GetConsoleMode() failed\n");
    }

    if (GetConsoleMode (GetStdHandle (STD_OUTPUT_HANDLE), &old_output_mode) == 0)
    {
        fprintf (stdout, "GetConsoleMode() failed\n");
    }

    SetConsoleMode (GetStdHandle (STD_INPUT_HANDLE), input_flags);
    SetConsoleMode (GetStdHandle (STD_OUTPUT_HANDLE), output_flags);
}

void disable_raw_mode (void)
{
    SetConsoleMode (GetStdHandle (STD_INPUT_HANDLE), old_input_mode);
    SetConsoleMode (GetStdHandle (STD_OUTPUT_HANDLE), old_output_mode);
}

int main (int argc, char **argv)
{
    int character = 0, count = 0;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    enable_raw_mode();

    while (character != 0x20) /* stop when space is received */
    {
        character = fgetc (stdin);
        
        fprintf (stdout, "%02X", character);
        if ((++count % 20) == 0)
            fprintf (stdout,"\r\n");
        else
            fprintf (stdout, " ");

        fflush (stdout);
    }

    disable_raw_mode();

    return 0;
}
