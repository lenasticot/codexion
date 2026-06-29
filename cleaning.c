/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleaning.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 16:47:47 by leodum            #+#    #+#             */
/*   Updated: 2026/06/29 14:55:04 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int join_threads(pthread_t thread)
{
	if (pthread_join(thread, NULL) != 0)
		return 1;
	printf("Coder thread returned succesfully\n");
	return 0;
}

int join_monitor(pthread_t monitor)
{
	if (pthread_join(monitor, NULL) != 0)
		return 1;
	printf("Monitor thread returned succesfully\n");
	return 0;
}

int free_coders(t_coder *coder)
{
	int i;
	
	i = 0;
	while(i < coder->sim->nb_coders)
	{
		pthread_mutex_destroy(&coder[i].CoderLock);
		i++;
	}
	free(coder);
	return 0;	
}

int free_sim(t_sim *sim)
{
	pthread_mutex_destroy(&sim->print_message);
	free(sim->args);
	free(sim->dongles);
	return 0;
}

void free_dongles_and_heap(t_sim *sim)
{
	int i;

	i = 0;

	while(i < sim->nb_coders)
	{
		// not sure if i need the if statement
		if(sim->dongles[i].heap)
		{
			free(sim->dongles[i].heap->arr);
			free(sim->dongles[i].heap);
		}
			
		pthread_mutex_destroy(&sim->dongles[i].DongleLock);
		pthread_cond_destroy(&sim->dongles[i].condDongle);
		i++;
	}
}

int free_them_all(t_sim *sim, t_coder *coder)
{
	free_dongles_and_heap(sim);
	free_coders(coder);
	free_sim(sim);
	free(sim);
	//pthread_mutex_destroy(&sim->print_message);
	//free(sim->args);
	//free(sim);
	return 0;
}