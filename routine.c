/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:38:25 by leodum            #+#    #+#             */
/*   Updated: 2026/05/27 16:19:01 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int print_status(t_coder *coder, char *message)
{
	if (!check_simulation_ongoing(coder->sim))
		return 1;
    pthread_mutex_lock(&coder->sim->print_message);
    printf("%ld %i %s\n", elapsed_ms(coder->sim->start_time), coder->nb, message);
	pthread_mutex_unlock(&coder->sim->print_message);
	return 0;
}

int take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->DongleLock);
	insertKey(dongle->heap, coder);
	coder->priority_rank++;
	while (1)
	{
		if (!check_simulation_ongoing(coder->sim))
		{
			pthread_mutex_unlock(&dongle->DongleLock);
			return 1;
		}
		if (dongle->status == 0 
			&& dongle->heap->arr[0].nb == coder->nb 
			&& dongle->available_to_use <= get_time_ms())
		{
			dongle->status = 1;
			coder->nb_dongle++;
			removeMin(dongle->heap);
			pthread_mutex_unlock(&dongle->DongleLock);
			print_status(coder, "has taken a dongle");
			return 0;
		}
		else
		// still a bit unsure about the wait
			pthread_cond_wait(&dongle->condDongle, &dongle->DongleLock);
	}
	return 1;
}

void release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->DongleLock);
	dongle->status = 0;
	coder->nb_dongle--;
	dongle->available_to_use = dongle->time_to_cooldown + get_time_ms();
	pthread_mutex_unlock(&dongle->DongleLock);
	pthread_cond_broadcast(&dongle->condDongle);
}

void routine_process(t_coder *coder)
{
	coder->last_time_compiled = get_time_ms();
	print_status(coder, "is COMPILING");
	usleep(coder->args->time_to_compile * 1000);
	// do i need to lock last_time_compiled?
	// i need to lock it everytime there is a chance i can use it somewhere else

	coder->nb_of_compiles--;
	release_dongle(coder, coder->l_dongle);
	release_dongle(coder, coder->r_dongle);
	print_status(coder, "is debuging");
	usleep(coder->args->time_to_debug * 1000);
	print_status(coder, "is refactoring");
	usleep(coder->args->time_to_refactor * 1000);
}

int dongle_management(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle)
{
	while(coder->nb_of_compiles != 0)
	{
		coder->nb_dongle = 0;
		if (l_dongle->rank > r_dongle->rank)
		{
			if(take_dongle(coder, l_dongle) == 1)
				return 1;
			if(take_dongle(coder, r_dongle) == 1)
				return 1;
		}
		else
		{
			if(take_dongle(coder, r_dongle) == 1)
				return 1;
			if(take_dongle(coder, l_dongle) == 1)
				return 1;
		}
		if (coder->nb_dongle == 2)
			routine_process(coder);
	}		
	return 0;
}
