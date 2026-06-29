/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 17:18:07 by leodum            #+#    #+#             */
/*   Updated: 2026/05/27 16:19:22 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int check_simulation_ongoing(t_sim *sim)
{
	int ret;

	pthread_mutex_lock(&sim->print_message);
	ret = (sim->ongoing != 1);
	pthread_mutex_unlock(&sim->print_message);
	return ret;
}

int check_compilation_nb(t_sim *sim)
{
	int i;
	int compiles;

	i = 0;
	while(i < sim->args->nb_coders)
	{
		pthread_mutex_lock(&sim->coder[i].CoderLock);
		compiles = sim->coder[i].nb_of_compiles;
		pthread_mutex_unlock(&sim->coder[i].CoderLock);
		if (compiles != 0)
			return 0;
		i++;
	}
	return 1;
}

static void stop_simulation(t_sim *sim)
{
	int i;

	pthread_mutex_lock(&sim->print_message);
	sim->ongoing = 1;
	pthread_mutex_unlock(&sim->print_message);
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].DongleLock);
		pthread_cond_broadcast(&sim->dongles[i].condDongle);
		pthread_mutex_unlock(&sim->dongles[i].DongleLock);
		i++;
	}
}

void* monitor_routine(void *monitor)
{
	t_sim *sim = (t_sim *) monitor;
	int i;
	long int last;
	long int avail;

	print_status(&sim->coder[0], "Monitor time");
	while(1)
	{
		usleep(1000);
		i = 0;
		while(i < sim->nb_coders)
		{
			pthread_mutex_lock(&sim->coder[i].CoderLock);
			last = sim->coder[i].last_time_compiled;
			pthread_mutex_unlock(&sim->coder[i].CoderLock);
			if (get_time_ms() - last > sim->args->time_to_burnout)
			{
				print_status(&sim->coder[i], "has burnout");
				stop_simulation(sim);
				return NULL;
			}
			pthread_mutex_lock(&sim->dongles[i].DongleLock);
			avail = sim->dongles[i].available_to_use;
			pthread_mutex_unlock(&sim->dongles[i].DongleLock);
			if (avail <= get_time_ms())
			{
				pthread_mutex_lock(&sim->dongles[i].DongleLock);
				pthread_cond_broadcast(&sim->dongles[i].condDongle);
				pthread_mutex_unlock(&sim->dongles[i].DongleLock);
			}
			i++;
		}
		if (check_compilation_nb(sim) == 1)
		{
			stop_simulation(sim);
			return NULL;
		}
	}
	return NULL;
}
