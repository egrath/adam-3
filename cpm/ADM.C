#include "stdio.h"

#define TERM_COLUMNS	80
#define TERM_ROWS		24

#define ASCII_SPACE		0x20

#define ASCII_SUB		0x1A
#define ASCII_ESC		0x1B


/* ########################################################
 * ## ADM-3A control functions                           ##
 * ######################################################## */

void cls ()
{
	fprintf (stdout, "%c", ASCII_SUB);
}

void cursor (row, column)
int row;
int column;
{
	fprintf (stdout, "%c=%c%c", ASCII_ESC, row+0x20-1,column+0x20-1);
}

void t_bg ()
{
	fprintf (stdout, "%c%c", ASCII_ESC, ')');
}

void t_fg ()
{
	fprintf (stdout, "%c%c", ASCII_ESC, '(');
}

void wait ()
{
	cursor (TERM_ROWS,25);
	fprintf (stdout, "Press any key to continue ...");
	getchar ();
}

/* ########################################################
 * ## TESTS                                              ##
 * ######################################################## */

void test01 ()
{
	int i;
	
	cls ();
	
	for (i=1; i <= TERM_ROWS; i ++)
	{
		cursor (i, 1);
		fprintf (stdout, "%02d:",i);
	}
	
	cursor (5, 20);
	fprintf (stdout, "This should be at row 5, column 20");
}

void test02 ()
{
	cls ();
	
	cursor (3, 5);
	fprintf (stdout, "Normaler Text ");
	t_bg ();
	fprintf (stdout, "Inverser Text");
	t_fg ();
	fprintf (stdout, " Normaler Text");
}

void test03 ()
{
	cls ();
	
	cursor (3,5);
	fprintf (stdout, "%c%c%d", ASCII_ESC, 'G', 0);
	fprintf (stdout, "Normaler Text");
	
	cursor (5,15);
	fprintf (stdout, "%c%c%d", ASCII_ESC, 'G', 4);
	fprintf (stdout, "Inverser Text");
	
	cursor (7,25);
	fprintf (stdout, "%c%c%d", ASCII_ESC, 'G', 8);
	fprintf (stdout, "Unterstrichener Text");
	
	cursor (9,35);
	fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '<');
	fprintf (stdout, "Unterstrichener und Inverser Text");
	
	fprintf (stdout, "%c%c%d", ASCII_ESC, 'G', 0);
}

main()
{
	/*
	test01 ();
	wait ();
	test02 ();
	wait ();
	*/
	
	test03 ();
	wait ();
}