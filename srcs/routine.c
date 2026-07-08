/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:38:25 by leodum            #+#    #+#             */
/*   Updated: 2026/07/08 17:47:38 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_lock);
	insert_key(dongle->heap, coder);
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

void	release_dongles(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle)
{
	pthread_mutex_lock(&l_dongle->dongle_lock);
	l_dongle->status = 0;
	l_dongle->available_to_use = l_dongle->time_to_cooldown + get_time_ms();
	pthread_cond_broadcast(&l_dongle->cond_dongle);
	pthread_mutex_unlock(&l_dongle->dongle_lock);
	pthread_mutex_lock(&r_dongle->dongle_lock);
	r_dongle->status = 0;
	r_dongle->available_to_use = r_dongle->time_to_cooldown + get_time_ms();
	pthread_cond_broadcast(&r_dongle->cond_dongle);
	pthread_mutex_unlock(&r_dongle->dongle_lock);
	coder->nb_dongle = 0;
}


void	routine_process(t_coder *coder)
{
	usleep(coder->args->time_to_compile * 1000);
	release_dongles(coder, coder->l_dongle, coder->r_dongle);
	print_status(coder, "is debugging");
	usleep(coder->args->time_to_debug * 1000);
	print_status(coder, "is refactoring");
	usleep(coder->args->time_to_refactor * 1000);
}


static int	dongle_ready(t_coder *coder, t_dongle *d)
{
	return (d->status == 0 && d->heap->arr[0].nb == coder->nb
		&& d->available_to_use <= get_time_ms());
}

static int	try_take_both(t_coder *coder, t_dongle *ld, t_dongle *rd)
{
	pthread_mutex_lock(&ld->dongle_lock);
	pthread_mutex_lock(&rd->dongle_lock);
	if (dongle_ready(coder, ld) && dongle_ready(coder, rd))
	{
		ld->status = 1;
		rd->status = 1;
		remove_min(ld->heap);
		remove_min(rd->heap);
		coder->nb_dongle = 2;
		pthread_mutex_unlock(&rd->dongle_lock);
		pthread_mutex_unlock(&ld->dongle_lock);
		print_status(coder, "has taken a dongle");
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(&coder->coder_lock);
		coder->last_time_compiled = get_time_ms();
		coder->nb_of_compiles--;
		pthread_mutex_unlock(&coder->coder_lock);
		print_status(coder, "is compiling");
		return (1);
	}
	else
	{
		pthread_mutex_unlock(&rd->dongle_lock);
		pthread_mutex_unlock(&ld->dongle_lock);
	}
	return (0);
}
int enque_dongle(t_coder *coder)
{
	pthread_mutex_lock(&coder->l_dongle->dongle_lock);
	insert_key(coder->l_dongle->heap, coder);
	pthread_mutex_unlock(&coder->l_dongle->dongle_lock);
	pthread_mutex_lock(&coder->r_dongle->dongle_lock);
	insert_key(coder->r_dongle->heap, coder);
	pthread_mutex_unlock(&coder->r_dongle->dongle_lock);
	return (0);
}

int	dongle_management_v2(t_coder *coder)
{
	if (coder->l_dongle == coder->r_dongle)
		return (0);
	while (coder->nb_of_compiles > 0)
	{
		enque_dongle(coder);
		while (!try_take_both(coder, coder->r_dongle, coder->l_dongle))
		{
			if (!check_simulation_ongoing(coder->sim))
				return (1);
			usleep(500);
		}
	routine_process(coder);
	}
	return (0);
}

// int	dongle_management(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle)
// {
// 	if (coder->nb % 2 == 0)
// 		usleep(1000);
// 	while (coder->nb_of_compiles != 0)
// 	{
// 		coder->nb_dongle = 0;
// 		if (coder->nb % 2 == 0)
// 		{
// 			if (take_dongle(coder, l_dongle) == 1)
// 				return (1);
// 			if (take_dongle(coder, r_dongle) == 1)
// 				return (1);
// 		}
// 		else
// 		{
// 			if (take_dongle(coder, r_dongle) == 1)
// 				return (1);
// 			if (take_dongle(coder, l_dongle) == 1)
// 				return (1);
// 		}
// 		if (coder->nb_dongle == 2)
// 			routine_process(coder);
// 	}
// 	return (0);
// }
