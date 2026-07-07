/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 16:43:36 by leodum            #+#    #+#             */
/*   Updated: 2026/06/29 17:52:34 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

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

int	check_priority_order(char *args)
{
	if (ft_strlen(args) != 3 && ft_strlen(args) != 4)
		return (0);
	if (ft_strlen(args) == 3)
	{
		if (!ft_strcmp(args, "edf"))
			return (0);
	}
	else if (ft_strlen(args) == 4)
	{
		if (!ft_strcmp(args, "fifo"))
			return (0);
	}
	return (1);
}

int	is_valid_number(char *args)
{
	int	i;

	i = 0;
	while (args[i] == ' ' || args[i] == '\t')
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
	if (!check_priority_order(str[i]))
		return (0);
	return (1);
}
