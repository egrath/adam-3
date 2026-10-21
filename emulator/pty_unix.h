#ifndef PTY_UNIX_H
#define PTY_UNIX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <signal.h>

#include <string.h>

#include "commandline.h"
#include "adm3a.h"
#include "pty.h"

#define MAX_SLAVE_NAME      64

/* private functions */
static int     pty_master_open (char *slaveName, size_t len);
static pid_t   pty_fork (int *masterFd, struct termios *slaveTermios, const struct winsize *slaveWs);
static void    enable_raw_mode (void);
static void    disable_raw_mode (void);

/* public functions are imported from pty.h because we have different implementations
   on Windows and Macintosh / Linux */

#endif
