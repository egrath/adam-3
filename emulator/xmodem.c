#include "xmodem.h"

bool xmodem_send (const char *s)
{
    FILE *f;
    uint8_t *block, *chunk;
    int total_blocks = 0, current_block = 0;
    bool finished = false;
    enum XModemState state = XM_WAIT_NAK, last_state = XM_UNDEFINED;

    if ((f = fopen (s, "rb")) == NULL)
    {
        fprintf (stdout, "XMODEM: failed to open source file\n");
        return false;
    }
    else
        total_blocks = file_blocks (f);
    fprintf (stdout, "XMODEM: file has a total number of %d blocks\n", total_blocks);

    block = (uint8_t *) malloc (sizeof (uint8_t) * XMODEM_BLOCKSIZE);

    while (!finished)
    {
        switch (state)
        {
            case XM_WAIT_NAK:
                if (!wait_nak ())
                {
                    fprintf (stdout, "XMODEM: not receiving initial NAK, aborting\n");
                    return false;
                }
                state = XM_SEND_BLOCK;
                break;

            case XM_WAIT_ACK:
                if (wait_ack ())
                {
                    /* we were waiting for the final ack? */
                    if (last_state == XM_SEND_EOT)
                        finished = true;
                    else
                    {
                        /* next block */
                        current_block ++;
                        if (current_block >= total_blocks)
                            state = XM_SEND_EOT;
                        else
                            state = XM_SEND_BLOCK;
                    }
                }
                else
                    state = XM_SEND_BLOCK;
                break;

            case XM_SEND_EOT:
                break;

            case XM_SEND_BLOCK:
                /* build the transmission block */
                block[0] = 0x01;                /* SOH */
                block[1] = current_block;
                block[2] = 255 - block[1];

                /* get the 128-byte block from the file ("chunk") and copy it to
                   the transmission block */
                chunk = get_file_block (f, current_block);
                memcpy (block+3, chunk, 128);

                block[131] = calculate_checksum (chunk);
                free (chunk);

                /* transfer the block */
                fprintf (stdout, "XMODEM: Transfering block %d, waiting for ACK\n", current_block);
                write_child ((char *) block, XMODEM_BLOCKSIZE);
                state = XM_WAIT_ACK;
                break;

            default:
                break;
        }
    }

    return true;
}

bool xmodem_receive (void)
{
    return true;
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

bool wait_nak (void)
{
    bool have_nak = false;
    char buffer[PTY_BUFFER_SIZE];
    int num_buffer_read, i;

    while (!have_nak)
    {
        num_buffer_read = read_child (buffer, PTY_BUFFER_SIZE);

        if (num_buffer_read > 0)
        {
            fprintf (stdout, "XMODEM: read %d bytes from child\n", num_buffer_read);

            for (i = 0; i < num_buffer_read; i ++)
            {
                if (buffer[i] == 0x15)
                    have_nak = true;
            }
        }
    }

    fprintf (stdout, "XMODEM: received initial NAK, continuing\n");
    return true;
}

bool wait_ack (void)
{
    bool have_ack = false, have_nak = false;
    char buffer[PTY_BUFFER_SIZE];
    int num_buffer_read, i;

    while (!have_ack && !have_nak)
    {
        num_buffer_read = read_child (buffer, PTY_BUFFER_SIZE);

        if (num_buffer_read > 0)
        {
            fprintf (stdout, "XMODEM: read %d bytes from child\n", num_buffer_read);

            for (i = 0; i < num_buffer_read; i ++)
            {
                if (buffer[i] == 0x06)
                {
                    fprintf (stdout, "XMODEM: got an ACK, good!\n");
                    have_ack = true;
                }
                else if (buffer[i] == 0x15)
                {
                    fprintf (stdout, "XMODEM: got an NACK, resending!\n");
                    have_nak = true;
                }
            }
        }
    }

    return have_ack;
}
