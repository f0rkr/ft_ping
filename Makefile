FLAGS=-Wall -Werror -Wextra -fsanitize=address -g
CC=gcc
EXEC_NAME=ft_ping
SRCS=srcs/main.c srcs/parsing/parse_clo.c srcs/handle_errors.c srcs/icmp_utils.c srcs/handle_time.c
INCLUDES=includes/


all: $(EXEC_NAME)


$(EXEC_NAME): $(SRCS)
	@$(CC) $(FLAGS) $(SRCS) -I $(INCLUDES) -o $(EXEC_NAME)

clean:
	@rm -rf $(EXEC_NAME)

fclean: clean


re: fclean all
