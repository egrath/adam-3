#ifndef XMODEM_H
#define XMODEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pty_unix.h"

enum XModemState
{
    XM_UNDEFINED,
    XM_WAIT_NAK,
    XM_WAIT_ACK,
    XM_SEND_BLOCK,
    XM_SEND_EOT
};

#define XMODEM_BLOCKSIZE       132

bool xmodem_send (const char *s);
bool xmodem_receive (void);

static bool wait_nak (void);
static bool wait_ack (void);

static uint8_t calculate_checksum (uint8_t *block);
static int file_blocks (FILE *f);
static uint8_t * get_file_block (FILE *f, int block_number);

#endif
