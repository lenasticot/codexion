/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 17:18:07 by leodum            #+#    #+#             */
/*   Updated: 2026/05/26 16:49:49 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int check_simulation_ongoing(t_sim *sim)
{
	if (sim->ongoing == 1)
		return 0;
	return 1;
}

int check_compilation_nb(t_sim *sim)
{
	int i = 0;
	while(i < sim->args->nb_coders)
	{
		if (sim->coder[i].nb_of_compiles != 0)
			return 0;
		i++;	
	}
	return 1;
}

void* monitor_routine(void *monitor)
{
	t_sim *sim = (t_sim *) monitor;
	int i = 0;
	int j = 0;
	printf("Monitor routine is launched correctly\n");
	while(1)
	{
		usleep(1000);
		
		while(i < sim->nb_coders)
		{
			if (get_time_ms() - sim->coder[i].last_time_compiled  >= sim->args->time_to_burnout)
			{
				print_status(&sim->coder[i], "has burnout");
				// do i need the mutex then if i print outside?
				// to think about it
				sim->ongoing = 1;
				i = 0;
				while(i < sim->nb_coders)
				{
					pthread_cond_broadcast(&sim->dongles[i].condDongle);	
					i++;
				}
				return NULL;
			}
			if (sim->dongles[j].available_to_use <= get_time_ms())
			{
				pthread_cond_broadcast(&sim->dongles[j].condDongle);
			}
			i++;
			j++;
		}
		if(check_compilation_nb(sim) == 1)
		{
			sim->ongoing = 1;
			i = 0;
			while(i < sim->nb_coders)
			{
				pthread_cond_broadcast(&sim->dongles[i].condDongle);	
				i++;
			}
			return NULL;
		}	
		i = 0;	
		j = 0;	
	}
	return NULL;
}
