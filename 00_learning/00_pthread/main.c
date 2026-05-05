#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* creating_coder(void *i)
{
	int *index = (int *) i;
	int check = 0;
	printf("%i Creating coder %i\n", check, *index);
}

int main(int argc, char **argv)
{
	int coders = 40;
	pthread_t t[coders];
	int i = 0;

	while (i != coders)
	{
		if (pthread_create(&t[i], NULL, &creating_coder, (void *) &i) != 0)
			return 1;
		i++;
	}
	i = 0;
	while (i != coders)
	{
		if (pthread_join(t[i], NULL) != 0)
			return 1;
		i++;
	}
	return 0;
}
