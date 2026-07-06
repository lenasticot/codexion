# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: leodum <leodum@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/27 12:40:03 by leodum            #+#    #+#              #
#    Updated: 2026/07/06 16:56:16 by leodum           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= codexion
 
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread
 
SRCS	= main.c \
		  init.c \
		  parsing.c \
		  routine.c \
		  heap.c \
		  sim.c \
		  cleaning.c \
		  utils.c
 
OBJS	= $(SRCS:.c=.o)

all: $(NAME)
 
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
 
%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@
 
test: $(NAME)
	@chmod +x tester_2.sh
	@./tester_2.sh
 
run: $(NAME)
	./$(NAME) $($(TEST))
 
clean:
	rm -f $(OBJS)
 
fclean: clean
	rm -f $(NAME)
 
re: fclean all

.PHONY: all clean fclean re test run