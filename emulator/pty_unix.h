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

#include <string.h>

#define MAX_SLAVE_NAME      64
#define PTY_BUFFER_SIZE     1024

int ptyMasterOpen (char *slaveName, size_t len);
pid_t ptyFork (int *masterFd, struct termios *slaveTermios, const struct winsize *slaveWs);
void enableRawMode (void);
void disableRawMode (void);



#endif
