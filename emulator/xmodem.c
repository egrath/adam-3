#include "xmodem.h"

bool xmodem_in_progress = false;

/* we save the status of the xmodem transfer to know if it was successful or not */
enum XModemStatus xmodem_status;

/* state machine status */
static enum XModemState xmodem_state;

void xmodem_begin (enum XModemTransferType type, const char *file)
{
    fprintf (stdout, "XMODEM: starting xmodem transfer: %s\n", type == XMT_SEND ? "Send" : "Receive");
    
    switch (type)
    {
        case XMT_SEND:
            xmodem_state = XM_SEND_WAIT_INITIAL_NAK;
            xmodem_in_progress = true;
            break;

        default:
            xmodem_state = XM_INITIAL;
            break;
    }
}

void xmodem_end (void)
{
    fprintf (stdout, "XMODEM: ending xmodem transfer\n");

    xmodem_in_progress = false;
}

bool xmodem_step (void)
{
    bool rc = false;
    char input;
    int read;

    switch (xmodem_state)
    {
        case XM_SEND_WAIT_INITIAL_NAK:
            read = read_child (&input, 1);
            if (read == 1)
            {
            }
            else
            {
            }
            break;

        default:
            xmodem_state = XM_INITIAL;
            xmodem_status = XMS_UNKNOWN;
            xmodem_in_progress = false;
            break;
    }

    return rc;
}

/* ########################################################
   ## Private functions                                  ##
   ######################################################## */

/* return a pointer to a block of data from the file, specified
   by the block_number. Callee is responsible for freeing the
   block. Why? Because we may need to pad (the last block).

   block_numnber is from 0 to total_blocks-1 */
uint8_t * get_file_block (FILE *f, int block_number)
{
    long offset = 128 * block_number;
    uint8_t *block;
    int num_read;

    block = (uint8_t *) malloc (sizeof (uint8_t) * 128);
    memset (block, 0x1A, 128); /* pre-pad */

    fseek (f, offset, SEEK_SET);
    num_read = fread (block, 1, 128, f);

    fprintf (stdout, "XMODEM: read block from file, size was %d bytes\n", num_read);

    return block;
}

/* return the total number of blocks in the file */
int file_blocks (FILE *f)
{
    long current, size;
    int blocks;

    /* size of file */
    current = ftell (f);
    fseek (f, 0, SEEK_END);
    size = ftell (f);
    fseek (f, current, SEEK_SET);

    fprintf (stdout, "XMODEM: file size is %lu bytes\n", size);

    /* how many blocks? */
    blocks = (int) (size / 128);
    fprintf (stdout, "%d\n", blocks);
    if (((size - (blocks * XMODEM_BLOCKSIZE)) % XMODEM_BLOCKSIZE) > 0)
        blocks++;

    return blocks;
}

/* calculate the checksum for the block, which is the sum of
   all bytes modulo 256 */
uint8_t calculate_checksum (uint8_t *block)
{
    uint32_t sum = 0;
    int i;

    for (i = 0; i < 128; i ++)
        sum += block[i];

    /* sum mod 256 */
    return (sum & 0x000000FF);
}
