/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 12:34:58 by leodum            #+#    #+#             */
/*   Updated: 2026/05/20 12:47:41 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

void swap(int *x, int *y);


struct MinHeap
{
    int *harr; // pointer to array of elements in heap
    int capacity; // maximum possibile size of min heap
    int heap_size; // current nb of elements in min heap
};

void MinHeapify(struct MinHeap *h, int i);

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

int getMin(struct MinHeap *h)
{
	return h->harr[0];
}

struct MinHeap *createMinHeap(int capacity)
{
	struct MinHeap *h = (struct MinHeap *)malloc(sizeof(struct MinHeap));
	h->heap_size = 0;
	h->capacity = capacity;
	h->harr = (int *)malloc(capacity * sizeof(int));
	return h;
}
// insert a new key "k"
void insertKey(struct MinHeap *h, int k)
{
	int i;
	
	if (h->heap_size == h->capacity)
	{
		printf("\nOverflow: Could not insterKey\n");
		return ;
	}
	//insert new key at the end
	h->heap_size++;
	i = h->heap_size - 1;
	h->harr[i] = k;

	//fix the min heap porpoerty if it is violated
	while (i != 0 && h->harr[parent(i) > h->harr[i]])
	{
		swap(&h->harr[i], &h->harr[parent(i)]);
		i = parent(i);
	}
}

// Decreases value of key at idex "i" to new_val
void decreaseKey(struct MinHeap *h, int i, int new_val)
{
	h->harr[i] = new_val;
	while(i != 0 && h->harr[parent(i)] > h->harr[i])
	{
		swap(&h->harr[i], &h->harr[parent(i)]);
		i = parent(i);
	}
}

//method to remove minimum elelemt (or root) from min heap
// need to work on the return INT_MAX;
int extractMin(struct MinHeap *h)
{
	int root;
	if (h->heap_size <= 0)
		return INT_MAX;
	if (h->heap_size == 1)
	{
		h->heap_size--;
		return h->harr[0];
	}
	root = h->harr[0];
	h->harr[0] = h->harr[h->heap_size - 1];
	h->heap_size--;
	MinHeapify(h, 0);

	return root;
}
// delete key at index i
void deleteKey(struct MinHeap *h, int i)
{
	decreaseKey(h, i, INT_MIN);
	extractMin(h);
}

// recursive method to heapify a subtree with root at given index
void MinHeapify(struct MinHeap *h, int i)
{
	int l = left(i);
	int r = right(i);
	int smallest = i;
	
	if (l < h->heap_size && h->harr[l] < h->harr[i])
		smallest = l;
	if (r < h->heap_size && h->harr[r] < h->harr[smallest])
		smallest = r;
	if (smallest != i)
	{
		swap(&h->harr[i], &h->harr[smallest]);
		MinHeapify(h, smallest);
	}
}

void swap(int *x, int *y)
{
	int tmp = *x;
	*x = *y;
	*y = tmp;
}


int main()
{
    struct MinHeap *h = createMinHeap(11);
    insertKey(h, 3);
    insertKey(h, 2);
    deleteKey(h, 1);
    insertKey(h, 15);
    insertKey(h, 5);
    insertKey(h, 4);
    insertKey(h, 45);
    printf("%d ", extractMin(h));
    printf("%d ", getMin(h));
    decreaseKey(h, 2, 1);
    printf("%d", getMin(h));

    // Free allocated memory
    free(h->harr);
    free(h);
    return 0;
}

// void	build_stack(t_coder **a_tail, t_coder **a_head, char **argv)
// {
// 	int	i;
// 	int	node;
// 	int	start;

// 	i = 1;
// 	if (!ft_atol(argv[i], &start))
// 	{
// 		print_error();
// 		return ;
// 	}
// 	init_stack(a_tail, a_head, start, i);
// 	i++;
// 	while (argv[i])
// 	{
// 		if (!ft_atol(argv[i], &node))
// 		{
// 			free_stack(a_tail);
// 			print_error();
// 			return ;
// 		}
// 		add_node_below(node, i, a_head);
// 		i++;
// 	}
// }

// void	add_node_below(int data, int pos, t_coder **head)
// {
// 	t_coder	*new_node;

// 	new_node = malloc(sizeof(t_coder));
// 	if (new_node == NULL)
// 		return ;
// 	new_node->nb = i;
// 	new_node->priority_rank = i;
// 	new_node->nb_dongle = 0;
// 	new_node->nb_of_compiles = ft_atoi(argv[6]);
// 	new_node->time_to_burnout = ft_atoi(argv[2]) * 1000; 
// 	new_node->last_time_compiled = get_time_ms();
// 	new_node->l_dongle = &dongle[i];
// 	new_node->r_dongle = &dongle[(i - 1 + nb_coders) % nb_coders];
// 	new_node->sim = &sim;
// 	new_node->args = &args;
// 	new_node->prev = *head;
// 	new_node->next = NULL;
// 	(*head)->next = new_node;
// 	*head = new_node;
// }

// void	init_stack(t_coder **tail, t_coder **head, int value, int pos)
// {
// 	t_coder	*new_node;

// 	new_node = malloc(sizeof(t_coder));
// 	if (new_node == NULL)
// 		return ;
// 	new_node->nb = i;
// 	new_node->priority_rank = i;
// 	new_node->nb_dongle = 0;
// 	new_node->nb_of_compiles = ft_atoi(argv[6]);
// 	new_node->time_to_burnout = ft_atoi(argv[2]) * 1000; 
// 	new_node->last_time_compiled = get_time_ms();
// 	new_node->l_dongle = &dongle[i];
// 	new_node->r_dongle = &dongle[(i - 1 + nb_coders) % nb_coders];
// 	new_node->sim = &sim;
// 	new_node->args = &args;
// 	new_node->prev = NULL;
// 	new_node->next = NULL;
// 	*tail = new_node;
// 	*head = new_node;
// }

// void insert(int ele)
// {
// 	t_coder *tmp = malloc(sizeof(struct node*));

// 	if (temp == NULL)
// 		return NULL;
// 	temp->data = ele;
// 	temp->link = NULL;

// 	if (rear == NULL)
// 		front = rear = temp;
// 	else
// 	{
// 		rear->link = temp;
// 		rear = temp;
// 	}
// }

// void dele(int ele)
// {
// 	struct node* temp;
// 	temp = front;

// 	if (front == NULL)
// 	{
// 		printf("Queue underflow");
// 		front = rear = NULL;
// 	}
// 	else
// 	{
// 		printf("The deletec element is: %d\n", front->data);
// 		front = front->link;
// 		free(temp);
// 	}
// }

// void display(int ele)
// {
// 	int i = 0;
// 	struct node* tmp;
// 	tmp = front;

// 	if (front == NULL)
// 		printf("Queue Underflow");
// 	else
// 	{
// 		printf("The queue elements are: %d\n");
// 		while (tmp)
// 		{
// 			printf("%d", tmp->data);
// 			tmp = tmp->link;
// 			i++;
// 		}
// 	}
// }