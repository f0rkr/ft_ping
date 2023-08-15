FLAGS=-Wall -Werror -Wextra -fsanitize=address -g
CC=gcc
EXEC_NAME=ft_ping
SRCS=srcs/main.c srcs/parsing/parse_clo.c srcs/handle_errors.c srcs/icmp_utils.c srcs/handle_time.c
INCLUDES=includes/
IP_ADDRESS := $(shell ip addr | grep "enp1s0"|grep inet | cut -d ' ' -f6|cut -d '/' -f1)
IP := $(echo "HEHLLO")
all: $(EXEC_NAME)


$(EXEC_NAME): $(SRCS)
	echo $(IP_ADDRESS)
	$(CC) $(FLAGS) $(SRCS) -D SRC_ADDRESS=\"10.0.2.15\" -I $(INCLUDES) -o $(EXEC_NAME) 

clean:
	@rm -rf $(EXEC_NAME)

fclean: clean


re: fclean all
