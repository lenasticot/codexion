/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 17:18:07 by leodum            #+#    #+#             */
/*   Updated: 2026/06/29 18:53:40 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
// need to check this
int	check_simulation_ongoing(t_sim *sim)
{
	int ret;

	pthread_mutex_lock(&sim->print_message);
	ret = (sim->ongoing != 1);
	pthread_mutex_unlock(&sim->print_message);
	return ret;
}

int	check_compilation_nb(t_sim *sim)
{
	int	i;

	i = 0;
	while(i < sim->args->nb_coders)
	{
		pthread_mutex_lock(&sim->coder[i].CoderLock);
		if (sim->coder[i].nb_of_compiles != 0)
		{
			pthread_mutex_unlock(&sim->coder[i].CoderLock);
			return 0;
		}	
		pthread_mutex_unlock(&sim->coder[i].CoderLock);
		i++;
	}
	return 1;
}
// why static void
static void	stop_simulation(t_sim *sim)
{
	int	i;

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


int	check_burnout(t_sim *sim, int i)
{
	long int	last;
	
	pthread_mutex_lock(&sim->coder[i].CoderLock);
	last = sim->coder[i].last_time_compiled;
	pthread_mutex_unlock(&sim->coder[i].CoderLock);
	if (get_time_ms() - last > sim->args->time_to_burnout)
	{
		print_status(&sim->coder[i], "has burnout");
		stop_simulation(sim);
		return (1);
	}
	return 0;
}
void	check_avail_dongle(t_sim *sim, int i)
{
	pthread_mutex_lock(&sim->dongles[i].DongleLock);
	if (sim->dongles[i].available_to_use <= get_time_ms())
		pthread_cond_broadcast(&sim->dongles[i].condDongle);
	pthread_mutex_unlock(&sim->dongles[i].DongleLock);
}


void	*monitor_routine(void *monitor)
{
	t_sim	*sim; 
	int		i;

	sim = (t_sim *) monitor;
	while(1)
	{
		usleep(1000);
		i = 0;
		while(i < sim->nb_coders)
		{
			if (check_burnout(sim, i) == 1)
			return (NULL);
			check_avail_dongle(sim, i);
			i++;
		}
		if (check_compilation_nb(sim) == 1)
		{
			stop_simulation(sim);
			return (NULL);
		}
	}
}
