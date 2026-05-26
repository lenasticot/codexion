/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:30:06 by leodum            #+#    #+#             */
/*   Updated: 2026/05/26 17:22:21 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int init_coder(t_coder **coder, t_dongle *dongle, t_args *args, t_sim *sim, pthread_mutex_t **CoderLock)
{
	int i;

	i = 0;
	*CoderLock = malloc(args->nb_coders * sizeof(pthread_mutex_t));
	if (!*CoderLock)
		return 1;
	*coder = malloc(args->nb_coders * sizeof(t_coder));
	if (!*coder)
		return 1;
	// still need ot work on the init
	while (i < args->nb_coders)
	{
		(*coder)[i].nb = i + 1;
		(*coder)[i].priority_rank = 0;
		(*coder)[i].nb_dongle = 0;
		(*coder)[i].nb_of_compiles = args->nb_of_compiles;
		(*coder)[i].time_to_burnout = args->time_to_burnout * 1000; // do i need this here?
		(*coder)[i].last_time_compiled = get_time_ms();
		(*coder)[i].l_dongle = &dongle[i];
		(*coder)[i].r_dongle = &dongle[(i - 1 + args->nb_coders) % args->nb_coders];
		(*coder)[i].sim = sim; 
		(*coder)[i].args = args;
		if (pthread_mutex_init(&(*coder)[i].CoderLock, NULL) != 0)
			return 1;
		i++;
	}
	sim->coder = *coder;
	return 0;
}

int init_dongle(char **argv, int nb_coders, t_dongle **dongle, pthread_mutex_t **DongleLock)
{
	int i;

	i = 0;
	*dongle = malloc(nb_coders * sizeof(t_dongle));
	if (!*dongle)
		return 1;
	*DongleLock = malloc(nb_coders * sizeof(pthread_mutex_t));
	if (!*DongleLock)
		return 1;
	while (i < nb_coders)
	{
		(*dongle)[i].rank = i + 1;
		(*dongle)[i].status = 0;
		(*dongle)[i].time_to_cooldown = ft_atoi(argv[7]);
		printf("Dongle %d has been created\n", (*dongle)[i].rank);
		createHeap(&(*dongle)[i].heap, 2);
		if (pthread_mutex_init(&(*dongle)[i].DongleLock, NULL) != 0)
			return 1;
		pthread_cond_init(&(*dongle)[i].condDongle, NULL);
		i++;
	}
	return 0;
}

void init_args(char **argv, int nb_coders, t_args **args)
{
	*args = malloc(sizeof(t_args));
	if (!*args)
		return ;
	
	(*args)->nb_coders = nb_coders; // do i need to keep this??
	(*args)->time_to_burnout = ft_atoi(argv[2]) * 1000; 
	(*args)->time_to_compile = ft_atoi(argv[3]);
	(*args)->time_to_debug = ft_atoi(argv[4]);
	(*args)->time_to_refactor = ft_atoi(argv[5]);
	(*args)->nb_of_compiles = ft_atoi(argv[6]);
}

int init_sim(t_sim **sim, t_args *args, t_coder *coder, t_dongle *dongle)
{
	t_sim *new_sim;
	
	new_sim = malloc(sizeof(t_sim));
	if (!new_sim)
		return 1;
	new_sim->start_time = get_time_ms();
	new_sim->nb_coders = args->nb_coders;
	new_sim->coder = coder; // can i initialize to coder if it is not initialize yet, like just send to the adress, i think 
	new_sim->args = args;
	new_sim->ongoing = 0;
	new_sim->dongles = dongle;
	if (pthread_mutex_init(&new_sim->print_message, NULL) != 0)
		return 1;
	*sim = new_sim;
	return (0);
}

int launch_sim(t_args *args, t_coder *coder, t_sim *sim)
{
	int i;
	pthread_t *threads;
	pthread_t monitor;
	
	threads = malloc(args->nb_coders * sizeof(pthread_t));
	if (!threads)
		return 1;
	i = 0;
	while (i < args->nb_coders)
	{
		// a bit weird the launching routine to check whats going on
		printf("Coder %i is starting their routine\n", coder->nb);
		if (pthread_create(&threads[i], NULL, &launching_routine, (void *) &coder[i]) != 0)
			return 1;
		i++;
	}

	if (pthread_create(&monitor, NULL, &monitor_routine, (void *) sim) != 0)
		return 1;
	i = 0;
	while (i < args->nb_coders)
	{
		if (pthread_join(threads[i], NULL) != 0)
			return 1;
		printf("Thread %i returned\n", i +1);
		i++;
	}
	if (pthread_join(monitor, NULL) != 0)
		return 1;
	printf("Monitor thread returned\n");
	return 0;
}

int heap_init(t_entry **h_entry, t_args *args)
{
	*h_entry = (t_entry *)malloc(args->nb_coders * sizeof(t_entry));
	if (!h_entry)
		return 1;
}

void init_management(char **argv)
{
	int nb_coders;
	t_dongle *dongle;
	t_args *args;
	t_coder *coder;
	t_sim *sim;
	t_entry *h_entry;
	pthread_mutex_t *DongleMutex;
	pthread_mutex_t *CoderMutex;
	int i;

	i = 0;
	nb_coders = ft_atoi(argv[1]);
	init_args(argv, nb_coders, &args);
	heap_init(&h_entry, args);
	printf("dongle success %i\n", init_dongle(argv, nb_coders, &dongle, &DongleMutex));
	init_sim(&sim, args, coder, dongle);
	printf("coder success %i\n", init_coder(&coder, dongle, args, sim, &CoderMutex));
	launch_sim(args, coder, sim);
	return ;
}
