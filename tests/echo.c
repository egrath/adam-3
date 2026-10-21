#include <stdio.h>

int main (int argc, char **argv)
{
	char buffer[100];

	fprintf (stdout, "Echo client\n");
	fflush (stdout);

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);	

	for (;;)
	{
		fgets (buffer, 99, stdin);
		fprintf (stdout, "%s", buffer);
		fflush (stdout);
	}

	return 0;
}