/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:16:55 by leodum            #+#    #+#             */
/*   Updated: 2026/05/17 14:46:00 by leodum           ###   ########.fr       */
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
long int get_time_ms()
{
	struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_sec * 1000) + (t.tv_usec / 1000);
}

long int elapsed_ms(long int start_ms)
{
	return get_time_ms() - start_ms;
}

void* launching_routine(void *args)
{
	t_coder *coder = (t_coder *) args;
	printf("Coder %i will start their routine\n", coder->nb);
	dongle_management(coder, coder->l_dongle, coder->r_dongle);
	return NULL;
}

void* monitor_routine(void *monitor)
{
	// need to understand how to calculate burnout
	t_sim *sim = (t_sim *) monitor;
	int i = 0;
	while(1)
	{
		while(i <= sim->nb_coders)
		{
			if (sim->coder[i].time_to_burnout > elapsed_ms(sim->start_time))
			{
				printf("Coder X has burnout\n");
				sim->ongoing = 1;
				return 0;
			}

			i++;
		}
		printf("No one has burnout, continuing routine\n");
		i = 0;		
	}
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
	
	// creating coder struct
	int nb_coders = ft_atoi(argv[1]);
	t_args args;
	t_coder coder[nb_coders];
	pthread_mutex_t mutex[nb_coders];
	t_dongle dongle[nb_coders];
	t_sim sim;
	pthread_t threads[nb_coders];
	long i = 0;
	// initializing args
 	args.nb_coders = nb_coders;
	args.time_to_burnout = ft_atoi(argv[2]); //might need to check the time and the ms thingy here
	args.time_to_compile = ft_atoi(argv[3]);
	args.time_to_debug = ft_atoi(argv[4]);
	args.time_to_refactor = ft_atoi(argv[5]);
	args.time_to_cooldown = ft_atoi(argv[7]);
	// initializing coders /dongle
	while (i < nb_coders)
	{
		coder[i].nb = i;
		coder[i].priority_rank = i;
		coder[i].nb_dongle = 0;
		coder[i].nb_of_compiles = ft_atoi(argv[6]);
		coder[i].time_to_burnout = ft_atoi(argv[2]); 
		coder[i].l_dongle = &dongle[i];
		coder[i].r_dongle = &dongle[(i - 1 + nb_coders) % nb_coders];
		coder[i].sim = &sim;
		coder[i].args = &args;
		dongle[i].rank = i;
		dongle[i].status = 0;
		if (pthread_mutex_init(&dongle[i].lock, NULL) != 0)
			return 1;
		pthread_cond_init(&dongle[i].condDongle, NULL);
		i++;
	}
	i = 0;
	// initializing sim thread
	sim.start_time = get_time_ms();
	sim.nb_coders = nb_coders;
	sim.coder = coder;
	sim.args = &args;
	sim.ongoing = 0;
	if (pthread_mutex_init(&sim.print_message, NULL) != 0)
		return 1;
	//creating the monitor
	pthread_t monitor;
	if (pthread_create(&monitor, NULL, &monitor_routine, (void *) &sim) != 0)
		return 1;
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
	if (pthread_join(monitor, NULL) != 0)
		return 1;
	return 0;
}