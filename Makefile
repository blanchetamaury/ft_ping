C=cc

CFLAGS= -Wall -Werror -Wextra -g -Iinclude/

SRC= srcs/main.c srcs/print.c srcs/signal.c srcs/init.c

OBJ= $(SRC:%.c=%.o)

NAME=ft_ping

all: $(NAME)

$(NAME): $(OBJ)
	$(C) -o $(NAME) $^ $(FLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re