/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:16:55 by leodum            #+#    #+#             */
/*   Updated: 2026/07/08 16:42:17 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	stop_simulation(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->print_message);
	sim->ongoing = 1;
	pthread_mutex_unlock(&sim->print_message);
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].dongle_lock);
		pthread_cond_broadcast(&sim->dongles[i].cond_dongle);
		pthread_mutex_unlock(&sim->dongles[i].dongle_lock);
		i++;
	}
}

void	*launching_routine(void *args)
{
	t_coder	*coder;

	coder = (t_coder *) args;
	dongle_management_v2(coder);
	return (NULL);
}

int	launch_sim(t_args *args, t_coder *coder, t_sim *sim)
{
	int			i;
	pthread_t	*threads;
	pthread_t	monitor;

	threads = malloc(args->nb_coders * sizeof(pthread_t));
	if (!threads)
		return (1);
	i = 0;
	while (i < args->nb_coders)
	{
		pthread_create(&threads[i], NULL,
			&launching_routine, (void *) &coder[i]);
		i++;
	}
	pthread_create(&monitor, NULL, &monitor_routine, (void *) sim);
	join_monitor(monitor);
	i = 0;
	while (i < args->nb_coders)
		join_threads(threads[i++]);
	free(threads);
	return (0);
}

void	init_management(char **argv)
{
	int			nb_coders;
	t_dongle	*dongle;
	t_args		*args;
	t_coder		*coder;
	t_sim		*sim;

	coder = NULL;
	nb_coders = atoi(argv[1]);
	init_args(argv, nb_coders, &args);
	init_dongle(argv, nb_coders, &dongle);
	init_sim(&sim, args, coder, dongle);
	init_coder(&coder, dongle, args, sim);
	launch_sim(args, coder, sim);
	free_them_all(sim, coder);
	return ;
}

int	main(int argc, char **argv)
{
	if (atoi(argv[6]) == 0 || atoi(argv[1]) == 0)
	{
		printf("Number provided cannot be 0, provide a valid number\n");
		return (1);
	}
		
	if (argc != 9)
	{
		printf("You need to provide 7 int +"
			"type of priority check (e.g. fifo or edf)\n");
		return (1);
	}
	if (!only_int_allowed(argv))
	{
		printf("Invalid arguments\n");
		return (1);
	}
	init_management(argv);
	return (0);
}
