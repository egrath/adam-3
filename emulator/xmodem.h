#ifndef XMODEM_H
#define XMODEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pty_unix.h"

enum XModemState
{
    XM_INITIAL,
    XM_SEND_WAIT_INITIAL_NAK
};

enum XModemStatus
{
    XMS_UNKNOWN,
    XMS_FINISHED
};

enum XModemTransferType
{
    XMT_SEND,
    XMT_RECEIVE
};

#define XMODEM_BLOCKSIZE        132
#define XMODEM_CHUNKSIZE        128

extern bool xmodem_in_progress;
extern enum XModemStatus xmodem_status;

void xmodem_begin (enum XModemTransferType type, const char *file);
void xmodem_end (void);

bool xmodem_step (void);

static uint8_t calculate_checksum (uint8_t *block);
static int file_blocks (FILE *f);
static uint8_t * get_file_block (FILE *f, int block_number);

#endif
