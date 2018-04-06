NAME		= minishell
SRC			=	builtins.c \
				char_array.c \
				child_argv.c \
				exec.c \
				fp_arrays.c \
				file_op.c \
				history.c \
				history_keys.c \
				input.c \
				kv_array.c \
				main.c \
				path_utils.c \
				quotes.c \
				special_keys.c \
				string_utils.c \
				tab_key.c \
				termcaps.c
#				words.c

OBJ			= $(SRC:%.c=%.o)
CC			= gcc
FLAGS		= -g -Wextra -Wall -Werror # -fsanitize=address
INC			= -I libft/ -I libft/ft_printf/
LIB			= -L libft/ -lft -L libft/ft_printf/ -lftprintf -ltermcap

all:$(NAME)

$(NAME):
	make -C libft/
	make -C libft/ft_printf/
	$(CC) $(FLAGS) $(INC) -c $(SRC)
	$(CC) $(FLAGS) $(OBJ) $(LIB) -o $(NAME)

rmobj:
	rm -f $(OBJ)

rmbin:
	rm -f $(NAME)

again: rmobj rmbin all

clean: rmobj
	make clean -C libft/
	make clean -C libft/ft_printf/

fclean: clean rmbin
	make fclean -C libft/
	make fclean -C libft/ft_printf/

re: fclean all
