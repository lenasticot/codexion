#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>


void* creating_coder(void *i)
{
	pthread_mutex_t c_lock = PTHREAD_MUTEX_INITIALIZER;
	long index = (long) i;
	pthread_mutex_lock(&c_lock);
	printf("Creating coder %ld\n", index);
	pthread_mutex_unlock(&c_lock);
	return (void *)(index + 1);
}

int main(int argc, char **argv)
{
	int coders = 40;
	pthread_t t[coders];
	long i = 0;
	

	while (i != coders)
	{
		if (pthread_create(&t[i], NULL, &creating_coder, (void *) i) != 0)
			return 1;
		i++;
	}
	i = 0;
	void* result;
	while (i != coders)
	{
		if (pthread_join(t[i], &result) != 0)
			return 1;
		printf("Thread %ld returned %ld\n", i, (long)result);
		i++;
	}
	return 0;
}
