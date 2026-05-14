/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:38:25 by leodum            #+#    #+#             */
/*   Updated: 2026/05/14 17:00:06 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int take_dongle(t_coder *coder, t_dongle *dongle, pthread_cond_t condDongle)
{
	while(coder->nb_of_compiles != 0)
	{
			pthread_mutex_lock(&dongle->lock);
			while(dongle->status == 1)
			{
				printf("Coder %i is waiting for its other dongle...\n", coder->nb);
				pthread_cond_wait(&condDongle, &dongle->lock);
				
			}
				printf("%i has taken a dongle\n", coder->nb);
				dongle->status = 1;
				coder->nb_dongle++;
			pthread_mutex_unlock(&dongle->lock);
	}
}


int release_dongle(t_coder *coder, t_dongle *dongle, pthread_cond_t condDongle)
{
	
	pthread_mutex_lock(&dongle->lock);
	dongle->status = 0;
	coder->nb_dongle--;
	pthread_mutex_unlock(&dongle->lock);
	pthread_cond_broadcast(&condDongle);
}

int dongle_management(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle)
{
	pthread_cond_t condDongle;
	
	while(coder->nb_of_compiles != 0)
	{
		
		take_dongle(coder, l_dongle, condDongle);		
		take_dongle(coder, r_dongle, condDongle);
		if (coder->nb_dongle == 2)
		{
			printf("%i is compiling\n", coder->nb);
			sleep(3);
			coder->nb_of_compiles--;
			release_dongle(coder, l_dongle, condDongle);
			release_dongle(coder, r_dongle, condDongle);
		}
	}		
	return 0;
}

int compiling(t_coder *coder)
{
	// if coder.dongle == 2
	// start compiling
	// once debuging is done
	// release both dongle
	// go debuging or refactoring
	return 0;
}

int debuging(t_coder *coder)
{
	// start debuging
	// check if possible to take dongle
	return 0;
}

int refactoring(t_coder *coder)
{
	return 0;
}
