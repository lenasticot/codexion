/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:16:55 by leodum            #+#    #+#             */
/*   Updated: 2026/05/27 14:51:45 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "codexion.h"

int	ft_atoi(const char *nptr)
{
	int	i;
	int	result;
	int	sign;

	result = 0;
	i = 0;
	sign = 1;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign *= -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + (nptr[i] - '0');
		i++;
	}
	return (result * sign);
}

long int get_time_ms()
{
	struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_sec * 1000) + (t.tv_usec / 1000);
}

long int elapsed_ms(long int start_ms)
{
	return get_time_ms() - start_ms;
}

void* launching_routine(void *args)
{
	t_coder *coder = (t_coder *) args;
	printf("Coder %i will start their routine\n", coder->nb);
	dongle_management(coder, coder->l_dongle, coder->r_dongle);
	return NULL;
}


int main(int argc, char **argv)
{
	if (argc != 9)
	{
		printf("You need to provide 7 int + type of priority check (e.g. fifo or edf)\n");
		return (1);
	}
	if (!only_int_allowed(argv))
	{
		printf("Invalid arguments\n");
		return (1);
	}
	printf("The program is correct and is about to start\n");

	init_management(argv);


	return 0;
}
