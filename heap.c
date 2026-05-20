/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 12:44:08 by leodum            #+#    #+#             */
/*   Updated: 2026/05/20 19:11:23 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "codexion.h"

int parent(int i)
{
	return (i - 1) / 2;
}

int left(int i)
{
	return (2 * i + 1);
}

int right(int i)
{
	return (2 * i + 2);
}

t_coder getMin(t_heap *c)
{
	return c->arr[0];
}


void createHeap(t_heap **c, int capacity)
{
	*c = (t_heap *)malloc(sizeof(t_heap));
		if (!*c)
			return ;
		(*c)->arr = (t_coder *)malloc(capacity * sizeof(t_coder));
		if (!(*c)->arr)
			return ;
		(*c)->capacity = capacity;
		(*c)->heap_size = 0;
}

// maybe better to put coder here no?
void insertKey(t_heap *c, t_coder coder)
{
	int i;
	int parent;
	t_coder tmp;
	
	if(c->heap_size == c->capacity)
	{
		printf("\nOverflow: canot insert another key\n");
		return ;
	}
	c->heap_size++;
	i = c->heap_size - 1;
	c->arr[i] = coder;
	// not too bad, need to initialize priority rank
	while (i != 0)
	{
		parent = (i - 1) / 2;
		if (c->arr[parent].priority_rank > c->arr[i].priority_rank)
			return ;
		tmp = c->arr[parent];
		c->arr[parent] = c->arr[i];
		c->arr[i] = tmp;
		i = parent;
	}
}

void removeMin(t_heap *c)
{
	int root;
	if (c->heap_size <= 0)
		return ; // heap is empty
	if (c->heap_size > 0)
		c->arr[0] = c->arr[c->heap_size - 1];
	c->heap_size--;
	MinHeapify(c, 0);
}

// do i need a heapify given that it is just a 2 data heap struct
void MinHeapify(t_heap *c, int i)
{
	int l = left(i);
	int r = right(i);
	int smallest = i;
	t_coder tmp;

	if (l < c->heap_size && c->arr[l].priority_rank < c->arr[i].priority_rank)
		smallest = l;
	if (r < c->heap_size && c->arr[r].priority_rank < c->arr[smallest].priority_rank)
		smallest = r;
	if (smallest != i)
	{
		// need to work on this swap
		tmp = c->arr[smallest];
		c->arr[smallest] = c->arr[i];
		c->arr[i] = tmp;
		MinHeapify(c, smallest);
	}
}