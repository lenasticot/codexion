/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:38:25 by leodum            #+#    #+#             */
/*   Updated: 2026/06/30 16:33:04 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_lock);
	insert_key(dongle->heap, coder);
	coder->priority_rank++;
	while (1)
	{
		if (!check_simulation_ongoing(coder->sim))
		{
			pthread_mutex_unlock(&dongle->dongle_lock);
			return (1);
		}
		if (dongle->status == 0
			&& dongle->heap->arr[0].nb == coder->nb
			&& dongle->available_to_use <= get_time_ms())
		{
			dongle->status = 1;
			coder->nb_dongle++;
			remove_min(dongle->heap);
			pthread_mutex_unlock(&dongle->dongle_lock);
			print_status(coder, "has taken a dongle");
			return (0);
		}
		else
			pthread_cond_wait(&dongle->cond_dongle, &dongle->dongle_lock);
	}
	return (1);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_lock);
	dongle->status = 0;
	coder->nb_dongle--;
	dongle->available_to_use = dongle->time_to_cooldown + get_time_ms();
	pthread_cond_broadcast(&dongle->cond_dongle);
	pthread_mutex_unlock(&dongle->dongle_lock);
}

void	routine_process(t_coder *coder)
{
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_time_compiled = get_time_ms();
	pthread_mutex_unlock(&coder->coder_lock);
	print_status(coder, "is compiling");
	usleep(coder->args->time_to_compile * 1000);
	pthread_mutex_lock(&coder->coder_lock);
	coder->nb_of_compiles--;
	pthread_mutex_unlock(&coder->coder_lock);
	release_dongle(coder, coder->l_dongle);
	release_dongle(coder, coder->r_dongle);
	print_status(coder, "is debuging");
	usleep(coder->args->time_to_debug * 1000);
	print_status(coder, "is refactoring");
	usleep(coder->args->time_to_refactor * 1000);
}

int	dongle_management(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle)
{
	while (coder->nb_of_compiles != 0)
	{
		coder->nb_dongle = 0;
		if (l_dongle->rank > r_dongle->rank)
		{
			if (take_dongle(coder, l_dongle) == 1)
				return (1);
			if (take_dongle(coder, r_dongle) == 1)
				return (1);
		}
		else
		{
			if (take_dongle(coder, r_dongle) == 1)
				return (1);
			if (take_dongle(coder, l_dongle) == 1)
				return (1);
		}
		if (coder->nb_dongle == 2)
			routine_process(coder);
	}
	return (0);
}
