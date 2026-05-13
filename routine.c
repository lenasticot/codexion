/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:38:25 by leodum            #+#    #+#             */
/*   Updated: 2026/05/13 17:28:42 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int dongle_management(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle)
{
	// can add release dongle also but maybe to see that later

	// you could also check for like what are left and right doing
	// and move over to something else directly if they are already compiling
	// but are they keeping the dongle if they are doing smething else?
	// Maybe, then that means its not needed

	// so yesterday i learned that doing those checked might actually create some race conditions
	// the check here is wrong because the status nb are wrong
 
		// check if dongle is taken ? 
		// or just try to grab it and if not possible it would wait?
		// could be nice to have something to says if not possible
	while(coder->nb_of_compiles != 0)
	{
			pthread_mutex_lock(&l_dongle->lock);
			if (l_dongle->status == 0)
			{
				printf("%i has taken a dongle\n", coder->nb);
				l_dongle->status = 1;
				coder->nb_dongle++;
			}
			pthread_mutex_unlock(&l_dongle->lock);		

		
			pthread_mutex_lock(&r_dongle->lock);
			if (r_dongle->status == 0)
			{
			printf("%i has taken a dongle\n", coder->nb);
			r_dongle->status = 1;
			coder->nb_dongle++;
			}
			pthread_mutex_unlock(&r_dongle->lock);			

		if (coder->nb_dongle == 2)
		{
			printf("%i is compiling\n", coder->nb);
			sleep(3);
			coder->nb_of_compiles--;
			pthread_mutex_lock(&r_dongle->lock);
			r_dongle->status = 0;
			pthread_mutex_unlock(&r_dongle->lock);
			pthread_mutex_lock(&l_dongle->lock);
			l_dongle->status = 0;
			pthread_mutex_unlock(&l_dongle->lock);
			coder->nb_dongle = 0;
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
