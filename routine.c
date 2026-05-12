/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:38:25 by leodum            #+#    #+#             */
/*   Updated: 2026/05/12 16:26:52 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int dongle_management(t_coder *coder, t_dongle *dongle)
{
	// can add release dongle also but maybe to see that later
	// need to initialize and free the lock as well
	// need to initialize the dongle in the main

	// you could also check for like what are left and right doing
	// and move over to something else directly if they are already compiling
	// but are they keeping the dongle if they are doing smething else?
	// Maybe, then that means its not needed
	if (coder->l_dongle == 0 && dongle->status == 0)
	{
		pthread_mutex_lock(&dongle->lock);
		printf("%i has taken a dongle\n", coder->nb);
		coder->l_dongle = 1;
		dongle->status = 1;
		pthread_mutex_unlock(&dongle->lock);		
	}
	if (coder->r_dongle == 0)
	{
		pthread_mutex_lock(&dongle->lock);
		printf("%i has taken a dongle\n", coder->nb);
		coder->r_dongle == 1;
		dongle->status == 1;
		pthread_mutex_unlock(&dongle->lock);	
	}
	// if coder.dongle == 0 || 1
		// if right dongle free
			// take it
			// check how many dongle
			// if 2 go compiling
			// else
				// if left dongle free
					//take it
				// else
					// compile or refactor
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
