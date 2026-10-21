#ifndef PTY_WINDOWS_H
#define PTY_WINDOWS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "commandline.h"

/* this file contains the public functions we have to implement */
#include "pty.h"

/* private functions */
static char * get_error_message (DWORD error);
int		child_data_available (void);

/* public functions */
bool    child_has_data (void);
int     child_read (uint8_t *buffer, int buffer_size);
bool    child_write (uint8_t *buffer, int buffer_size);
bool    child_alive (void);
bool    start_process (void);
void    stop_process (void);

#endif