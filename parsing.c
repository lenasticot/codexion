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
	// if negative no 
	// if longer then int should fail
	
#include <stdio.h>

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int ft_strncmp(const char *s1, const char *s2)
{
	int i;

	i = 0;
	while (s1[i])
	{
		if (s1[i] == s2[i])
			i++;
		else 
			return (0);
	}
	if (!s1[i])
		return (1);
	return (0);  
}

int check_priority_order(char *args)
{
	int i;
	i = 0;
	if (ft_strlen(args) != 3 && ft_strlen(args) != 4)
		return (0);
	if (ft_strlen(args) == 3)
	{
		if (!ft_strncmp(args, "edf"))
			return (0);
	}
	else if (ft_strlen(args) == 4)
	{
		if (!ft_strncmp(args, "fifo"))
			return (0);
	}
	return (1);
}


int is_valid_number(char *args)
{
	int i;
	
	i = 0;
	while(args[i] == ' ' || args[i] == '\t')
		i++;
	if (args[i] == '\0')
		return (0);
	if (args[i] == '+')
		i++;
	if (args[i] == '-')
		return (0);
	if (args[i] < '0' || args[i] > '9')
		return (0);
	while (args[i])
	{
		if (args[i] < '0' || args[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	only_int_allowed(char **str)
{
	int	i;

	i = 1;
	while (i < 8)
	{
		if (!is_valid_number(str[i]))
			return (0);
		i++;
	}
	if(!check_priority_order(str[i]))
		return (0);

	return (1);
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
	else
		printf("The program is working and is now going to start");
	return (0);
}