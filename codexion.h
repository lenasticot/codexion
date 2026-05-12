/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:17:04 by leodum            #+#    #+#             */
/*   Updated: 2026/05/12 16:33:21 by leodum           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */




#ifndef CODEXION_H
# define FT_PRINTF_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <stdint.h>
# include <sys/time.h>

// What do i need my coder to be
// need to learn about the heap


// need to work also on time and managing milliseconds

typedef struct dongle {
	int rank;
	// status: 0 if free / 1 if taken
	int status;
	pthread_mutex_t lock;
} t_dongle;

typedef struct coder {
	// Just the number of the coder but to see if this is interesting or not
	// ?
	int nb;
	// maybe need to do a hierachy between them especially to know which one has the priority
	// ?
	int priority_rank;
	// for status could be equivalent to compile debug and refactor
	// and to be equal to 0 1 or 2
	int status;
	// or they have individual debug, refac and compile
	// 0 when off
	// 1 when on
	// always need to check if the other is on 
	// need to see what could be the most efficient
	int time_to_burnout;
	int time_to_compile;
	int time_to_debug;
	int time_to_refactor;
	// to know how many time the coder has compiled already
	// to be decreased until 0
	int nb_of_compiles;
	int time_to_cooldown;

	// to check how many dongle they have in their hand
	// ? 
	int l_dongle;
	int r_dongle;
	t_dongle *dongle;
}	t_coder;

// To prevent coders from duplicating dongles, you must protect each dongle’s state
// with a mutex (pthread_mutex_t). A condition variable (pthread_cond_t) may be
// used to manage waiting queues.



// Logging must be serialized so that two messages never interleave on a single line
// (use a mutex to protect output).

size_t	ft_strlen(const char *s);
int ft_strcmp(const char *s1, const char *s2);
int check_priority_order(char *args);
int is_valid_number(char *args);
int only_int_allowed(char **str);

void* creating_coder(void *i);
int ft_atoi(const char *nptr);
int main(int argc, char **argv);

int dongle_management(t_coder *coder, t_dongle *dongle);



#endif
