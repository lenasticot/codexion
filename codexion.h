/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:17:04 by leodum            #+#    #+#             */
/*   Updated: 2026/07/03 12:50:09 by leodum           ###   ########.fr       */
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
# include <string.h>

typedef struct sim	t_sim;
typedef struct heap	t_heap;

typedef struct args
{
	int		nb_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		nb_of_compiles;
	char	*scheduler;
}	t_args;

typedef struct dongle
{
	int				rank;
	int				status;
	pthread_mutex_t	dongle_lock;
	pthread_cond_t	cond_dongle;
	int				time_to_cooldown;
	int				available_to_use;
	t_heap			*heap;
}	t_dongle;

typedef struct coder
{
	int				nb;
	int				priority_rank;
	int				nb_dongle;
	int				nb_of_compiles;
	long int		last_time_compiled;
	t_dongle		*l_dongle;
	t_dongle		*r_dongle;
	t_args			*args;
	t_sim			*sim;
	pthread_mutex_t	coder_lock;
}	t_coder;

typedef struct heap_entry
{
	int			nb;
	long int	priority_rank;
	long int	deadline;
}	t_entry;

typedef struct heap
{
	t_entry	*arr;
	int		capacity;
	int		heap_size;
}	t_heap;

typedef struct sim
{
	long int		start_time;
	int				nb_coders;
	pthread_mutex_t	print_message;
	t_coder			*coder;
	t_args			*args;
	int				ongoing;
	t_dongle		*dongles;
}	t_sim;

// parsing
size_t		ft_strlen(const char *s);
int			ft_strcmp(const char *s1, const char *s2);
int			check_priority_order(char *args);
int			is_valid_number(char *args);
int			only_int_allowed(char **str);

// utils
int			ft_atoi(const char *nptr);
long int	get_time_ms(void);
long int	elapsed_ms(long int start_ms);
void		*launching_routine(void *args);
int			print_status(t_coder *coder, char *message);

//free and clean
int			join_threads(pthread_t thread);
int			join_monitor(pthread_t monitor);
void		free_threads(pthread_t *threads, int i);
int			free_coders(t_coder *coder);
int			free_sim(t_sim *sim);
void		free_dongles_and_heap(t_sim *sim);
int			free_them_all(t_sim *sim, t_coder *coder);

//initialization
void		init_management(char **argv);
int			init_sim(t_sim **sim, t_args *args,
				t_coder *coder, t_dongle *dongle);
void		init_args(char **argv, int nb_coders, t_args **args);
int			init_dongle(char **argv, int nb_coders, t_dongle **dongle);
int			init_coder(t_coder **coder, t_dongle *dongle,
				t_args *args, t_sim *sim);
int			launch_sim(t_args *args, t_coder *coder, t_sim *sim);

// main
void		*launching_routine(void *args);
int			ft_atoi(const char *nptr);
int			main(int argc, char **argv);
void		stop_simulation(t_sim *sim);

// routine
int			dongle_management(t_coder *coder, t_dongle *l_dongle,
				t_dongle *r_dongle);
void		routine_process(t_coder *coder);
void		release_dongle(t_coder *coder, t_dongle *dongle);
int			take_dongle(t_coder *coder, t_dongle *dongle);

// sim
void		*monitor_routine(void *monitor);
int			check_compilation_nb(t_sim *sim);
int			check_simulation_ongoing(t_sim *sim);
int			check_burnout(t_sim *sim, int i);
void		check_avail_dongle(t_sim *sim, int i);

// heap management
void		swap(t_entry *a, t_entry *b);
void		insert_key(t_heap *c, t_coder *coder);
void		remove_min(t_heap *c);
void		create_heap(t_heap **c, int capacity);
void	ordering_key(int i, t_heap *c, long int child_val, long int parent_val);

#endif