/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 15:10:48 by leodum            #+#    #+#             */
/*   Updated: 2026/07/06 16:48:22 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	join_threads(pthread_t thread)
{
	if (pthread_join(thread, NULL) != 0)
		return (1);
	return (0);
}

int	join_monitor(pthread_t monitor)
{
	if (pthread_join(monitor, NULL) != 0)
		return (1);
	return (0);
}

long int	get_time_ms(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return ((t.tv_sec * 1000) + (t.tv_usec / 1000));
}

long int	elapsed_ms(long int start_ms)
{
	return (get_time_ms() - start_ms);
}

int	print_status(t_coder *coder, char *message)
{
	pthread_mutex_lock(&coder->sim->print_message);
	if (coder->sim->ongoing == 1)
	{
		pthread_mutex_unlock(&coder->sim->print_message);
		return (1);
	}
	printf("%ld %i %s\n",
		elapsed_ms(coder->sim->start_time), coder->nb, message);
	pthread_mutex_unlock(&coder->sim->print_message);
	return (0);
}
