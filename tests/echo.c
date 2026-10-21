#include <stdio.h>

int main (int argc, char **argv)
{
	char buffer[100];

	fprintf (stdout, "Echo client\n");

	for (;;)
	{
		fgets (buffer, 99, stdin);
		fprintf (stdout, "%s", buffer);
	}

	return 0;
}