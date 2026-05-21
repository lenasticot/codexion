/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:38:25 by leodum            #+#    #+#             */
/*   Updated: 2026/05/21 17:41:36 by leodum           ###   ########.fr       */
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
}

int take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	printf("adding key into the heap\n");
	insertKey(dongle->heap, *coder);
	while (1)
	{
		if (!check_simulation_ongoing(coder->sim))
		{
			pthread_mutex_unlock(&dongle->lock);
			return 1;
		}
		if (dongle->status == 0 && dongle->heap->arr[0].nb == coder->nb || dongle->heap->heap_size == 0)
		{
			dongle->status = 1;
			coder->nb_dongle++;
			printf("On top of the heap of dongle %i is coder %i\n", dongle->rank, dongle->heap->arr[0].nb);
			coder->priority_rank += 2;
			pthread_mutex_unlock(&dongle->lock);
			print_status(coder, "has taken a dongle");
			return 0;
		}
		else
		{
			printf("coder %d is waiting for its dongle %i\n", coder->nb, dongle->rank);
			pthread_cond_wait(&dongle->condDongle, &dongle->lock);
		}
			
	}
}
 

int cooling_down(t_dongle *dongle)
{
	usleep(dongle->time_to_cooldown * 1000);
	pthread_mutex_lock(&dongle->lock);
	dongle->status = 0;
	pthread_mutex_unlock(&dongle->lock);
	pthread_cond_broadcast(&dongle->condDongle);
}
int release_dongle(t_coder *coder, t_dongle *dongle)
{
	// maybe here something like if 2 release both to reduce nb of lines
	// problem here with the cooldown
	pthread_mutex_lock(&dongle->lock);
	dongle->status = 0;
	printf("Removing top coder from the heap %i\n", dongle->heap->arr[0].nb);
	removeMin(dongle->heap);
	printf("Now on top is: %d\n", dongle->heap->arr[0].nb);
	pthread_mutex_unlock(&dongle->lock);


}

int dongle_management(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle)
{
	while(coder->nb_of_compiles != 0)
	{
		coder->nb_dongle = 0;
		// resource hierarchy solution
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
		{
			print_status(coder, "is COMPILING");
			usleep(coder->args->time_to_compile * 1000);
			coder->last_time_compiled = get_time_ms();
			coder->nb_of_compiles--;
			printf("coder %i need to compile %i more\n", coder->nb, coder->nb_of_compiles);
			release_dongle(coder, l_dongle);
			release_dongle(coder, r_dongle);
			print_status(coder, "is debuging");
			usleep(coder->args->time_to_debug * 1000);
			print_status(coder, "is refactoring");
			usleep(coder->args->time_to_refactor * 1000);
		}
	}		
	return 0;
}
