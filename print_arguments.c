#include <stdio.h>

int main(int ac, char *av[])
{
	int i = 1;
	while (i < ac)
	{
		printf("index = %d, argument = %s\n", i, av[i]);
		i++;
	}
	return (0);
}
