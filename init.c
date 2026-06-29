/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:30:06 by leodum            #+#    #+#             */
/*   Updated: 2026/06/29 17:40:19 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_coder(t_coder **coder, t_dongle *dongle, t_args *args, t_sim *sim)
{
	int	i;

	i = 0;
	*coder = malloc(args->nb_coders * sizeof(t_coder));
	if (!*coder)
		return (1);
	while (i < args->nb_coders)
	{
		(*coder)[i].nb = i + 1;
		(*coder)[i].priority_rank = 0;
		(*coder)[i].nb_dongle = 0;
		(*coder)[i].nb_of_compiles = args->nb_of_compiles;
		(*coder)[i].last_time_compiled = get_time_ms();
		(*coder)[i].l_dongle = &dongle[i];
		(*coder)[i].r_dongle = &dongle[(i - 1 + args->nb_coders)
			% args->nb_coders];
		(*coder)[i].sim = sim;
		(*coder)[i].args = args;
		if (pthread_mutex_init(&(*coder)[i].CoderLock, NULL) != 0)
			return (1);
		i++;
	}
	sim->coder = *coder;
	return (0);
}

int	init_dongle(char **argv, int nb_coders, t_dongle **dongle)
{
	int	i;

	*dongle = malloc(nb_coders * sizeof(t_dongle));
	if (!*dongle)
		return (1);
	i = 0;
	while (i < nb_coders)
	{
		(*dongle)[i].rank = i + 1;
		(*dongle)[i].status = 0;
		(*dongle)[i].time_to_cooldown = ft_atoi(argv[7]);
		(*dongle)[i].available_to_use = 0;
		createHeap(&(*dongle)[i].heap, 2);
		if (pthread_mutex_init(&(*dongle)[i].DongleLock, NULL) != 0)
			return (1);
		pthread_cond_init(&(*dongle)[i].condDongle, NULL);
		i++;
	}
	return (0);
}

void	init_args(char **argv, int nb_coders, t_args **args)
{
	*args = malloc(sizeof(t_args));
	if (!*args)
		return ;
	(*args)->nb_coders = nb_coders;
	(*args)->time_to_burnout = ft_atoi(argv[2]);
	(*args)->time_to_compile = ft_atoi(argv[3]);
	(*args)->time_to_debug = ft_atoi(argv[4]);
	(*args)->time_to_refactor = ft_atoi(argv[5]);
	(*args)->nb_of_compiles = ft_atoi(argv[6]);
	(*args)->scheduler = argv[8];
}

// need to make sure that the sim starts at the right moment
int	init_sim(t_sim **sim, t_args *args, t_coder *coder, t_dongle *dongle)
{
	t_sim	*new_sim;

	new_sim = malloc(sizeof(t_sim));
	if (!new_sim)
		return (1);
	new_sim->start_time = get_time_ms();
	new_sim->nb_coders = args->nb_coders;
	new_sim->coder = coder;
	new_sim->args = args;
	new_sim->ongoing = 0;
	new_sim->dongles = dongle;
	pthread_mutex_init(&new_sim->print_message, NULL);
	*sim = new_sim;
	return (0);
}
