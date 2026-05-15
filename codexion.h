/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:17:04 by leodum            #+#    #+#             */
/*   Updated: 2026/05/15 16:59:03 by leodum           ###   ########.fr       */
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

// need to learn about the heap and queue


// need to work also on time and managing milliseconds
// do i need individual args or not
typedef struct args {
	int nb_coders; // might be good to keep the nb of coders so then easy to know how many are in the linked list?
	int time_to_burnout;
	int time_to_compile;
	int time_to_debug;
	int time_to_refactor;
	int nb_of_compiles;
	int time_to_cooldown;
} t_args;

// How does each thread know the simulation start time?
// How does a monitor detect that someone burned out?
// How do threads signal that the simulation should stop?
typedef struct sim {
	pthread_cond_t otherDongle;
} t_sim;

typedef struct dongle {
	int rank;
	// status: 0 if free / 1 if taken
	int status;
	pthread_mutex_t lock;
	pthread_cond_t condDongle;
} t_dongle;

typedef struct coder {
	// Just the number of the coder but to see if this is interesting or not
	int nb;
	// maybe need to do a hierachy between them especially to know which one has the priority
	// ?
	// might be what i will do in the heap and queue thing so this might not be that needed atm
	int priority_rank;
	int nb_dongle;
	// to check how many dongle they have in their hand
	// ? 
	t_dongle *l_dongle;
	t_dongle *r_dongle;
	t_args *args;
	t_sim *sim;
	// t_dongle *dongle;
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

void* launching_routine(void *args);
int ft_atoi(const char *nptr);
int main(int argc, char **argv);

int dongle_management(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle);



#endif
