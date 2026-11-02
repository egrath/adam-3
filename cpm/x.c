#include "stdio.h"

main ()
{
    fprintf (stdout, "%c", 0x1A); 
    fprintf (stdout, "%c=%c%c", 0x1B, 5+0x20-1, 5+0x20-1);
    fprintf (stdout, "ABC");
}
