# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: leodum <leodum@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/27 12:40:03 by leodum            #+#    #+#              #
#    Updated: 2026/07/06 18:44:25 by leodum           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread
INCLUDES = -I includes

SRC_DIR	= srcs
OBJ_DIR	= objs

SRCS	= main.c \
		  init.c \
		  parsing.c \
		  routine.c \
		  heap.c \
		  sim.c \
		  cleaning.c \
		  utils.c

SRC		= $(addprefix $(SRC_DIR)/, $(SRCS))
OBJ		= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

HEADER	= includes/codexion.h

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(NAME)
	@chmod +x testers/tester.sh
	@./testers/tester.sh

run: $(NAME)
	./$(NAME) $($(TEST))

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re test run