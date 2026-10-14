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

void header (s)
char *s;
{
	cursor (1, 10);
	if (s!=NULL)
		fprintf (stdout, "ADM-3A control sequence tester: %s", s);
}

/* ########################################################
 * ## TESTS                                              ##
 * ######################################################## */

void test01 (s)
char *s;
{
	int i;
	
	cls ();
	header(s);
	
	for (i=1; i <= TERM_ROWS; i ++)
	{
		cursor (i, 1);
		fprintf (stdout, "%02d:",i);
	}
	
	cursor (5, 20);
	fprintf (stdout, "This should be at row 5, column 20");
}

void test02 (s)
char *s;
{
	cls ();
	header(s);
	
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

void test03 (s)
char *s;
{
	cls();
	header(s);
	
	cursor (3,5);
	fprintf (stdout, "%c%c%d", ASCII_ESC, 'G', 2);
	fprintf (stdout, "Normal Blinkend");
	
	cursor (5,5);
	fprintf (stdout, "%c%c%d", ASCII_ESC, 'G', 6);
	fprintf (stdout, "Invers Blinkend");
	
	cursor (7,5);
	fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', ':');
	fprintf (stdout, "Unterstrichen Blinkend");
	
	cursor (9,5);
	fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '>');
	fprintf (stdout, "Unterstrichen Invers Blinkend");

	fprintf (stdout, "%c%c%d", ASCII_ESC, 'G', 0);
}

void test04 (s)
char *s;
{
	int i, col, row;
	
	cls ();
	header (s);
	
	row = 5, col = 5;
	
	cursor (row,col);
	for (i=32; i < 32+64; i++)
	{
		cursor (row,col);
		
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '0');
		fprintf (stdout, "%03d=[", i);
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', 'P');
		fprintf (stdout, "%c", i);
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '0');
		fprintf (stdout, "]");
		
		row ++;
		if (((i+1) % 8) == 0)
		{
			row = 5;
			col += 8;
		}
	}		
}

void test05 (s)
char *s;
{
	int i, col, row;
	
	cls ();
	header (s);
	
	row = 5, col = 5;
	
	cursor (row,col);
	for (i=32; i < 32+64; i++)
	{
		cursor (row,col);
		
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '0');
		fprintf (stdout, "%03d=[", i);
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', 'R');
		fprintf (stdout, "%c", i);
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '0');
		fprintf (stdout, "]");
		
		row ++;
		if (((i+1) % 8) == 0)
		{
			row = 5;
			col += 8;
		}
	}		
}

void test06 (s)
char *s;
{
	int i, col, row;
	
	cls ();
	header (s);
	
	row = 5, col = 5;
	
	cursor (row,col);
	for (i=32; i < 32+64; i++)
	{
		cursor (row,col);
		
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '0');
		fprintf (stdout, "%03d=[", i);
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', 'T');
		fprintf (stdout, "%c", i);
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '0');
		fprintf (stdout, "]");
		
		row ++;
		if (((i+1) % 8) == 0)
		{
			row = 5;
			col += 8;
		}
	}		
}

void test07 (s)
char *s;
{
	int i, col, row;
	
	cls ();
	header (s);
	
	row = 5, col = 5;
	
	cursor (row,col);
	for (i=32; i < 32+64; i++)
	{
		cursor (row,col);
		
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '0');
		fprintf (stdout, "%03d=[", i);
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', 'V');
		fprintf (stdout, "%c", i);
		fprintf (stdout, "%c%c%c", ASCII_ESC, 'G', '0');
		fprintf (stdout, "]");
		
		row ++;
		if (((i+1) % 8) == 0)
		{
			row = 5;
			col += 8;
		}
	}		
}


main()
{
	test01 ("cursor positioning");
	wait ();
	
	test02 ("text attributes 1");
	wait ();;
	
	test03 ("text attributes 2");
	wait ();

	test04 ("graphics mode");
	wait ();

	test05 ("graphics blinky mode");
	wait ();

	test06 ("graphics inverse mode");
	wait ();

	test07 ("graphics inverse blinky mode");
	wait ();
}
