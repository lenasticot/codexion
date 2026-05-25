/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:17:04 by leodum            #+#    #+#             */
/*   Updated: 2026/05/25 18:54:03 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CODEXION_H
# define CODEXION_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <stdint.h>
# include <sys/time.h>

typedef struct sim t_sim;
typedef struct heap t_heap;

typedef struct args {
	int nb_coders; 
	int time_to_burnout;
	int time_to_compile;
	int time_to_debug;
	int time_to_refactor;
} t_args;

typedef struct dongle {
	int rank;
	int status;
	pthread_mutex_t lock;
	pthread_cond_t condDongle;
	int time_to_cooldown;
	t_heap *heap;
} t_dongle;

typedef struct coder {
	int nb;
	int priority_rank;
	int nb_dongle;
 	int nb_of_compiles;
	int time_to_burnout;
	long int last_time_compiled; 
	t_dongle *l_dongle;
	t_dongle *r_dongle;
	t_args *args;
	t_sim *sim;
}	t_coder;

typedef struct heap_entry {
    int nb;
    int priority_rank;
} t_entry;

typedef struct heap {
	t_entry *arr;
	int capacity;
	int heap_size;
	
} t_heap;

typedef struct sim {
	long int start_time;
	int nb_coders;
	pthread_mutex_t print_message;
	t_coder *coder;  
	t_args *args; 
	int ongoing;
	t_dongle *dongles;
} t_sim;


size_t	ft_strlen(const char *s);
int ft_strcmp(const char *s1, const char *s2);
int check_priority_order(char *args);
int is_valid_number(char *args);
int only_int_allowed(char **str);

void* launching_routine(void *args);
int ft_atoi(const char *nptr);
int main(int argc, char **argv);
long int elapsed_ms(long int start_ms);
long int get_time_ms();
int print_status(t_coder *coder, char *message);

int dongle_management(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle);
void routine_process(t_coder *coder);

void *monitor_routine(void *monitor);
int check_compilation_nb(t_sim *sim);
int check_simulation_ongoing(t_sim *sim);


t_coder getMin(t_heap *c);
void swap(t_entry *a, t_entry *b);
void insertKey(t_heap *c, t_coder *coder);
void removeMin(t_heap *c);
void MinHeapify(t_heap *c, int i);
void createHeap(t_heap **c, int capacity);
#endif
