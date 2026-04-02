C=cc

CFLAGS= -Wall -Werror -Wextra -g -Iinclude/
ASAN_FLAGS = -fsanitize=address

SRC= srcs/main.c srcs/print.c srcs/signal.c srcs/init.c srcs/package.c srcs/parsing.c

OBJ= $(SRC:%.c=%.o)

NAME=ft_ping

all: $(NAME)

$(NAME): $(OBJ)
	$(C) -o $(NAME) $^ $(FLAGS)
	sudo setcap cap_net_raw+ep ./ft_ping

debug: $(OBJ)
	$(C) -o $(NAME) $^ $(FLAGS) $(ASAN_FLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re