/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:16:55 by leodum            #+#    #+#             */
/*   Updated: 2026/05/15 17:32:03 by leodum           ###   ########.fr       */
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


void* launching_routine(void *args)
{
	t_coder *coder = (t_coder *) args;
	printf("Coder %i will start their routine\n", coder->nb);
	dongle_management(coder, coder->l_dongle, coder->r_dongle);
	return NULL;
}

int main(int argc, char **argv)
{
	
	// getting time and converting it to mili
	// converting time to mili
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	long int mili = (current_time.tv_sec * 1000) + (current_time.tv_usec / 1000);
	printf("seconds: %ld\nmicro seconds: %ld\nmiliseconds: %ld\n", current_time.tv_sec, current_time.tv_usec, mili);
	
	// parsing part
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
	
	// creating coder struct
	int nb_coders = ft_atoi(argv[1]);
	t_args args[nb_coders];
	t_coder coder[nb_coders];
	pthread_mutex_t mutex[nb_coders];
	t_dongle dongle[nb_coders];
	t_sim sim;
	pthread_t threads[nb_coders];
	long i = 0;

	while (i < nb_coders)
	{
		coder[i].nb = i;
		coder[i].priority_rank = i;
		args[i].nb_coders = nb_coders;
		args[i].time_to_burnout = ft_atoi(argv[2]); //might need to check the time and the ms thingy here
		args[i].time_to_compile = ft_atoi(argv[3]);
		args[i].time_to_debug = ft_atoi(argv[4]);
		args[i].time_to_refactor = ft_atoi(argv[5]);
		args[i].nb_of_compiles = ft_atoi(argv[6]);
		args[i].time_to_cooldown = ft_atoi(argv[7]);
		coder[i].nb_dongle = 0;
		coder[i].l_dongle = &dongle[i];
		coder[i].r_dongle = &dongle[(i - 1 + nb_coders) % nb_coders];
		coder[i].sim = &sim;
		coder[i].args = &args[i];
		dongle[i].rank = i;
		dongle[i].status = 0;

		if (pthread_mutex_init(&dongle[i].lock, NULL) != 0)
			return 1;
		pthread_cond_init(&dongle[i].condDongle, NULL);
		i++;
	}
	i = 0;
	// sending the coder into each thread
	while (i < nb_coders)
	{
		if (pthread_create(&threads[i], NULL, &launching_routine, (void *) &coder[i]) != 0)
			return 1;
		i++;
	}
	i = 0;
	
	// closing the coder part
	while (i < nb_coders)
	{
		if (pthread_join(threads[i], NULL) != 0)
			return 1;
		printf("Thread %ld returned\n", i);
		i++;
	}
	struct timeval new_time;
	gettimeofday(&new_time, NULL);
	long int new_mili = (new_time.tv_sec * 1000) + (new_time.tv_usec / 1000);
	printf("seconds: %ld\nmicro seconds: %ld\nmiliseconds: %ld\n", new_time.tv_sec, new_time.tv_usec, new_mili);
	return 0;
}