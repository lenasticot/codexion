/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:30:06 by leodum            #+#    #+#             */
/*   Updated: 2026/05/27 17:13:38 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int init_coder(t_coder **coder, t_dongle *dongle, t_args *args, t_sim *sim)
{
	int i;

	i = 0;
	*coder = malloc(args->nb_coders * sizeof(t_coder));
	if (!*coder)
		return 1;
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

int init_dongle(char **argv, int nb_coders, t_dongle **dongle, pthread_t DongleLock)
{
	int i;


	i = 0;
	while (i < nb_coders)
	{
		(*dongle)[i].rank = i + 1;
		(*dongle)[i].status = 0;
		(*dongle)[i].time_to_cooldown = ft_atoi(argv[7]);
		(*dongle)[i].available_to_use = 0;
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
	(*args)->time_to_burnout = ft_atoi(argv[2]); 
	(*args)->time_to_compile = ft_atoi(argv[3]);
	(*args)->time_to_debug = ft_atoi(argv[4]);
	(*args)->time_to_refactor = ft_atoi(argv[5]);
	(*args)->nb_of_compiles = ft_atoi(argv[6]);
	(*args)->scheduler = argv[8];
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
		// printf("Coder %i is starting their routine\n", coder->nb);
		if (pthread_create(&threads[i], NULL, &launching_routine, (void *) &coder[i]) != 0)
		{
			free(threads);
			return 1;
		}
			
		i++;
	}
	if (pthread_create(&monitor, NULL, &monitor_routine, (void *) sim) != 0)
		return 1;
	i = 0;
	while (i < args->nb_coders)
	{
		join_threads(threads[i]);
		i++;
	}

	free(threads);
	join_monitor(monitor);
	free_them_all(sim, coder);
	return 0;
}

//int heap_init(t_entry **h_entry, t_args *args)
//{
//	*h_entry = (t_entry *)malloc(args->nb_coders * sizeof(t_entry));
//	if (!h_entry)
//		return 1;
//	return 0;
//}

void init_management(char **argv)
{
	int nb_coders;
	t_dongle *dongle;
	t_args *args;
	t_coder *coder;
	t_sim *sim;
	//t_entry *h_entry;
	pthread_t *DongleLock = NULL;
	//pthread_t CoderLock;

	coder = NULL;
	
	nb_coders = ft_atoi(argv[1]);
	printf("Args initialization\n");
	init_args(argv, nb_coders, &args);
	printf("Args initialized\n");
	//heap_init(&h_entry, args);
	printf("Dongle initialization\n");
	init_dongle(argv, nb_coders, &dongle, &DongleLock);
	printf("Dongle initializated\n");
	printf("Sim initialization\n");
	init_sim(&sim, args, coder, dongle);
	printf("Sim initializated\n");
	printf("Coders initialization\n");
	init_coder(&coder, dongle, args, sim);
	printf("Coders initializated\n");
	launch_sim(args, coder, sim);
	return ;
}
