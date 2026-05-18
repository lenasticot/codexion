/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leodum <leodum@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 12:17:04 by leodum            #+#    #+#             */
/*   Updated: 2026/05/18 14:41:23 by leodum           ###   ########.fr       */
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

typedef struct args {
	int nb_coders; 
	int time_to_burnout;
	int time_to_compile;
	int time_to_debug;
	int time_to_refactor;

	int time_to_cooldown;
} t_args;


typedef struct dongle {
	int rank;
	// status: 0 if free / 1 if taken
	int status;
	pthread_mutex_t lock;
	pthread_cond_t condDongle;
} t_dongle;

typedef struct coder {
	int nb;
	// maybe need to do a hierachy between them especially to know which one has the priority
	// ?
	// might be what i will do in the heap and queue thing so this might not be that needed atm
	int priority_rank;
	int nb_dongle;
 	int nb_of_compiles;
	int time_to_burnout;
	long int last_time_compiled; 
	t_dongle *l_dongle;
	t_dongle *r_dongle;
	t_args *args;
	t_sim *sim;
	// need to add smth like last time of compiling
	// then it will be my calculation for burnout
	// like if last time of compiling is 1234
	//burnout time is 400
	// then need to do last time of compiling + burnout time
	// = coder_burnout
	// and that should be what i check about in the monitor?
}	t_coder;

typedef struct sim {
	long int start_time;
	int nb_coders;
	pthread_mutex_t print_message;
	t_coder *coder;  
	t_args *args; 
	int ongoing;
	pthread_mutex_t lockBurnout;
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



#endif
