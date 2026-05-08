/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 16:43:36 by leodum            #+#    #+#             */
/*   Updated: 2026/05/08 17:09:43 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



// - Your program must take the following arguments (all mandatory):
	// 	`number_of_coders time_to_burnout time_to_compile time_to_debug 
	// time_to_refactor number_of_compiles_required dongle_cooldown scheduler`
	// check if number
	// if negative
	// if longer then int should fail
	


int initial_checks(int *args)
{
	if (!*args)
		return 1;
	else if (*args != 8)
		return 1;
}


int formating(int *args)
{
	int i = 0;
	while(args[i])
	{
		if (args[i] )
	}	
}
