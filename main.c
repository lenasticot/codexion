/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:16:55 by leodum            #+#    #+#             */
/*   Updated: 2026/05/11 12:54:41 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "codexion.h"

int	ft_atoi(const char *nptr)
{
	int	i;
	int	result;
	int	sign;

	result = 0;
	i = 0;
	sign = 1;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign *= -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + (nptr[i] - '0');
		i++;
	}
	return (result * sign);
}


void* creating_threads(void *args)
{
	t_coder *coder = (t_coder *) args;
	printf("Creating coder %i which time to burnout is now %i\n", coder->nb, coder->time_to_burnout);
	return NULL;
}

int main(int argc, char **argv)
{
	if (argc != 9)
	{
		printf("You need to provide 7 int + type of priority check (e.g. fifo or edf)\n");
		return (1);
	}

	if (!only_int_allowed(argv))
	{
		printf("Invalid arguments\n");
		return (1);
	}
	else
		printf("The program is working and is now going to start\n");
	

	int nb_coders = ft_atoi(argv[1]);
	t_coder coders[nb_coders];
	pthread_t threads[nb_coders];
	long i = 0;
	while (i < nb_coders)
	{
		coders[i].nb = i;
		coders[i].priority_rank = i;
		coders[i].status = 0; //0 = compile 1= debug 2=refactoring
		coders[i].time_to_burnout = ft_atoi(argv[2]); //might need to check the time and the ms thingy here
		coders[i].time_to_compile = ft_atoi(argv[3]);
		coders[i].time_to_debug = ft_atoi(argv[4]);
		coders[i].time_to_refactor = ft_atoi(argv[5]);
		coders[i].nb_of_compiles = ft_atoi(argv[6]);
		coders[i].time_to_cooldown = ft_atoi(argv[7]);
		coders[i].dongle = 0;
		i++;
	}
	i = 0;
	while (i < nb_coders)
	{
		if (pthread_create(&threads[i], NULL, &creating_threads, (void *) &coders[i]) != 0)
			return 1;
		i++;
	}
	i = 0;

	while (i < nb_coders)
	{
		if (pthread_join(threads[i], NULL) != 0)
			return 1;
		printf("Thread %ld returned\n", i);
		i++;
	}
	return 0;
}