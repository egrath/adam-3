#include <stdio.h>

int main (int argc, char **argv)
{
	char buffer[100];

	fprintf (stdout, "Echo client\n");
	fflush (stdout);

	for (;;)
	{
		fgets (buffer, 99, stdin);
		fprintf (stdout, "%s", buffer);
		fflush (stdout);
	}

	return 0;
}