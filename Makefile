#	if you're missing termcap.h on linux, install libncurses5-dev

NAME		=	minishell
SRCFILES	=	autocomplete.c \
				builtin_cd.c \
				builtins.c \
				builtins_env.c \
				char_array.c \
				child_argv.c \
				error.c \
				exec.c \
				file_op.c \
				history.c \
				history_keys.c \
				input_a.c \
				input_b.c \
				kv_array.c \
				main.c \
				norme.c \
				path_utils.c \
				quotes.c \
				special_keys_a.c \
				special_keys_b.c \
				string_utils.c \
				tab_columns.c \
				tab_key.c \
				tab_utils.c \
				termcaps.c

OBJFILES	=	$(SRCFILES:%.c=%.o)
SRC			=	$(addprefix src/,$(SRCFILES))
OBJ			=	$(addprefix obj/,$(OBJFILES))
CC			=	gcc
FLAGS		=	-Wextra -Wall -Werror -O2
INC			=	-I libft/inc -I inc/
LIB			=	-L libft/ -lft -ltermcap

all:$(NAME)

$(NAME): $(OBJ)
	make -C libft/
	$(CC) $(FLAGS) -o $@ $^ $(LIB)

objdir:
	mkdir -p obj/

obj/%.o: src/%.c | objdir
	$(CC) $(FLAGS) -c $< -o $@ $(INC)

rmobj:
	rm -rf obj/

rmbin:
	rm -rf $(NAME)

again: rmobj rmbin all

clean: rmobj
	make clean -C libft/

fclean: clean rmbin
	make fclean -C libft/

re: fclean all

