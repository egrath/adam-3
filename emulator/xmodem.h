#ifndef XMODEM_H
#define XMODEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool xmodem_send (const char *s);
bool xmodem_receive (void);

#endif
