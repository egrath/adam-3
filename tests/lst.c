#include <stdio.h>
#include <stdlib.h>

int numElements (char **list)
{
	int i = 0;

	while (*list != NULL)
	{
		i ++;
		list ++;
	}

	return i;
}

int main (int argc, char **argv)
{
	char *lst1[] = {"ABC","DEF","GHI","JKL","MNO","PQR","STU","VWX","YZ",NULL};
	char **lst2 = lst1;
	int i,j;

	i = numElements (lst1);
	fprintf (stdout, "# of elements: %d\n", i);

	for (j = 0; j < i; j ++)
	{
		fprintf (stdout, "%d = [%s]\n", j, *(lst2 + j));
	}

	return 0;
}

