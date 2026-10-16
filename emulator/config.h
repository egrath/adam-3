#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "helper.h"

typedef struct
{
    /* wrap cursor the next line after writing to column 80 */
    bool auto_newline;

    /* colors for the terminal */
    RGBAColor terminal_foreground;
    RGBAColor terminal_background;
    RGBAColor cursor_foreground;
    RGBAColor cursor_background;

    /* fullscreen mode */
    bool fullscreen;

    /* directory for receiving XMODEM files */
    char xmodem_recv_path[256];
}
configuration;

extern configuration config;

void config_set_default (void);
bool config_init (void);
void config_print (void);

#endif
