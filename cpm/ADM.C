#include "stdio.h"

#define TERM_COLUMNS	80
#define TERM_ROWS		24

#define ASCII_SPACE		0x20

#define ASCII_SUB		0x1A
#define ASCII_ESC		0x1B


/* ########################################################
 * ## ADM-3A control functions                           ##
 * ######################################################## */

void clear_screen ()
{
	fprintf (stdout, "%c", ASCII_SUB);
}

void set_cursor_at (row, column)
int row;
int column;
{
	fprintf (stdout, "%c=%c%c", ASCII_ESC, row+0x20-1,column+0x20-1);
}


/* ########################################################
 * ## TESTS                                              ##
 * ######################################################## */

void test01 ()
{
	int i;
	
	clear_screen ();
	
	for (i=1; i <= TERM_ROWS; i ++)
	{
		set_cursor_at (i, 1);
		fprintf (stdout, "%02d:",i);
	}
	
	set_cursor_at (5, 20);
	fprintf (stdout, "This should be at row 5, column 20");
}

void test02 ()
{
	clear_screen ();
}

void wait_next ()
{
	set_cursor (TERM_ROWS,25);
	fprintf (stdout, "Press any key to continue ...");
	getchar ();
	clear_screen ();
}

main()
{
	test01 ();
	wait_next ();
	test02 ();
	wait_next ();
}