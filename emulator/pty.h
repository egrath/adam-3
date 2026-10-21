#ifndef PTY_H
#define PTY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define PTY_BUFFER_SIZE     1024

/* public functions, have to be implemented in the operating system
   specific PTY handler */
bool    child_has_data (void);
int     child_read (uint8_t *buffer, int buffer_size);
bool    child_write (uint8_t *buffer, int buffer_size);
bool    child_alive (void);

bool    start_process (void);
void    stop_process (void);

#endif