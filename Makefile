# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: leodum <leodum@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/27 12:40:03 by leodum            #+#    #+#              #
#    Updated: 2026/05/27 17:16:22 by leodum           ###   ########.fr        #
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
		  cleaning.c 
 
OBJS	= $(SRCS:.c=.o)
 

 
TEST_BASIC		= 2 1505 300 300 300 5 50 fifo
TEST_BURNOUT	= 2 10  200 200 200 3 0   fifo
TEST_TIGHT		= 4 250 200 100 100 5 0   fifo
TEST_COOLDOWN	= 3 300 200 100 100 5 200 fifo
TEST_SINGLE		= 1 800 200 200 200 3 0   fifo
TEST_MANY		= 8 800 200 100 100 3 50  fifo
TEST_HIGH		= 2 800 200 100 100 10 0  fifo
TEST_FIFO		= 4 500 200 100 100 5 0   fifo
TEST_EDF		= 4 500 200 100 100 5 0   edf

 # ─── Stress tests ─────────────────────────────────────────────────────────────
TEST_STRESS_10		= 10  800 200 100 100 3 50  fifo
TEST_STRESS_20		= 20  800 200 100 100 3 50  fifo
TEST_STRESS_50		= 50  800 200 100 100 3 50  fifo
TEST_STRESS_100		= 100 800 200 100 100 3 50  fifo
TEST_STRESS_200		= 200 800 200 100 100 3 50  fifo

# Stress with tight burnout
TEST_STRESS_TIGHT	= 50  300 200 100 100 3 0   fifo
TEST_STRESS_BURN	= 100 250 200 100 100 3 50  fifo

# Stress EDF
TEST_STRESS_EDF_50	= 50  800 200 100 100 3 50  edf
TEST_STRESS_EDF_200	= 200 800 200 100 100 3 50  edf

# Stress with cooldown
TEST_STRESS_COOL	= 50  800 200 100 100 3 200 fifo
# ─────────────────────────────────────────────────────────────────────────────

all: $(NAME)
 
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
 
%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@
 
test: $(NAME)
	@echo "\n====== TEST 1: Basic 2 coders (should complete cleanly) ======"
	./$(NAME) $(TEST_BASIC)
	@echo "\n====== TEST 2: Burnout (should trigger fast) ======"
	./$(NAME) $(TEST_BURNOUT)
	@echo "\n====== TEST 3: Tight burnout window ======"
	./$(NAME) $(TEST_TIGHT)
	@echo "\n====== TEST 4: Cooldown induced burnout ======"
	./$(NAME) $(TEST_COOLDOWN)
	@echo "\n====== TEST 5: Single coder ======"
	./$(NAME) $(TEST_SINGLE)
	@echo "\n====== TEST 6: Many coders (8) ======"
	./$(NAME) $(TEST_MANY)
	@echo "\n====== TEST 7: High compile count ======"
	./$(NAME) $(TEST_HIGH)
	@echo "\n====== TEST 8: FIFO scheduler ======"
	./$(NAME) $(TEST_FIFO)
	@echo "\n====== TEST 9: EDF scheduler ======"
	./$(NAME) $(TEST_EDF)
 
test_stress: $(NAME)
	@echo "\n====== STRESS 10 coders ======"
	./$(NAME) $(TEST_STRESS_10)
	@echo "\n====== STRESS 20 coders ======"
	./$(NAME) $(TEST_STRESS_20)
	@echo "\n====== STRESS 50 coders ======"
	./$(NAME) $(TEST_STRESS_50)
	@echo "\n====== STRESS 100 coders ======"
	./$(NAME) $(TEST_STRESS_100)
	@echo "\n====== STRESS 200 coders ======"
	./$(NAME) $(TEST_STRESS_200)
	@echo "\n====== STRESS tight burnout 50 coders ======"
	./$(NAME) $(TEST_STRESS_TIGHT)
	@echo "\n====== STRESS burnout 100 coders ======"
	./$(NAME) $(TEST_STRESS_BURN)
	@echo "\n====== STRESS EDF 50 coders ======"
	./$(NAME) $(TEST_STRESS_EDF_50)
	@echo "\n====== STRESS EDF 200 coders ======"
	./$(NAME) $(TEST_STRESS_EDF_200)
	@echo "\n====== STRESS cooldown 50 coders ======"
	./$(NAME) $(TEST_STRESS_COOL)
 
run: $(NAME)
	./$(NAME) $($(TEST))
 
clean:
	rm -f $(OBJS)
 
fclean: clean
	rm -f $(NAME)
 
re: fclean all
 
.PHONY: all clean fclean re test run