/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 12:44:08 by leodum            #+#    #+#             */
/*   Updated: 2026/05/25 18:54:12 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "codexion.h"


void createHeap(t_heap **c, int capacity)
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

void swap(t_entry *a, t_entry *b)
{
	t_entry tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void orderingKey(int i, int parent, t_heap *c, long int coderChild, long int coderParent)
{
	if (coderParent < coderChild)
		return ;
	else if (coderParent == coderChild)
	{
		if (c->arr[parent].nb > c->arr[i].nb)
			return ;
	}
	else
		swap(&c->arr[parent], &c->arr[i]);
}

void insertKey(t_heap *c, t_coder *coder)
{
	int i;
	int parent;

	if(c->heap_size >= 2)
	{
		printf("\nOverflow: canot insert another key\n");
		return ;
	}
	c->heap_size++;
	i = c->heap_size - 1;
	c->arr[i].nb = coder->nb;
	c->arr[i].priority_rank = coder->priority_rank;
	c->arr[i].deadline = coder->last_time_compiled + coder->args->time_to_burnout;
	parent = (i - 1) / 2;
	if (parent == 0)
		return;
	if (!strcmp(coder->args->scheduler, "fifo"))
		orderingKey(i, parent, c, c->arr[i].priority_rank, c->arr[parent].priority_rank);
	else if (!strcmp(coder->args->scheduler, "edf"))
		orderingKey(i, parent, c, c->arr[i].deadline, c->arr[parent].deadline);
}

void removeMin(t_heap *c)
{
	t_entry *root;
	root = (t_entry *)malloc(sizeof (t_entry));
	if (!root)
		return ;
	*root = c->arr[0];
	c->arr[0] = c->arr[c->heap_size - 1];
	c->heap_size--;
	free(root);
}