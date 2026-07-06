/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 12:44:08 by leodum            #+#    #+#             */
/*   Updated: 2026/07/06 16:42:31 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	create_heap(t_heap **c, int capacity)
{
	*c = (t_heap *)malloc(sizeof(t_heap));
	if (!*c)
		return ;
	(*c)->arr = (t_entry *)malloc(capacity * sizeof(t_entry));
	if (!(*c)->arr)
		return ;
	(*c)->capacity = capacity;
	(*c)->heap_size = 0;
}

void	swap(t_entry *a, t_entry *b)
{
	t_entry	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	ordering_key(int i, t_heap *c, long int child_val, long int parent_val)
{
	int	parent;

	parent = (i - 1) / 2;
	if (parent_val < child_val)
		return ;
	else if (parent_val == child_val)
	{
		if (c->arr[parent].nb > c->arr[i].nb)
			return ;
	}
	else
		swap(&c->arr[parent], &c->arr[i]);
}

void	insert_key(t_heap *c, t_coder *coder)
{
	int	i;
	int	parent;

	if (c->heap_size >= 2)
	{
		printf("\nOverflow: canot insert another key\n");
		return ;
	}
	c->heap_size++;
	i = c->heap_size - 1;
	c->arr[i].nb = coder->nb;
	c->arr[i].priority_rank = get_time_ms();
	c->arr[i].deadline = coder->last_time_compiled
		+ coder->args->time_to_burnout;
	parent = (i - 1) / 2;
	if (!strcmp(coder->args->scheduler, "fifo"))
		ordering_key(i, c, c->arr[i].priority_rank,
			c->arr[parent].priority_rank);
	else if (!strcmp(coder->args->scheduler, "edf"))
		ordering_key(i, c, c->arr[i].deadline, c->arr[parent].deadline);
}

void	remove_min(t_heap *c)
{
	if (c->heap_size <= 0)
		return ;
	c->arr[0] = c->arr[c->heap_size - 1];
	c->heap_size--;
}
