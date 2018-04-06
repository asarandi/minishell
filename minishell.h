/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/30 19:50:54 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/06 03:30:58 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include "libft.h"
#include "libftprintf.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <termios.h>
#include <termcap.h>

typedef struct	s_av
{
	char	*in;
	char	*out;
	char	*key;
	char	*val;
}				t_av;


typedef struct	s_shell
{
	int			argc;
	char		**argv;
	char		**envp;
	char		*buffer;
	size_t		bufsize;
	size_t		input_size;
	size_t		buf_i;
	char		*partial_input;
	char		**history;
	int			history_count;
	int			history_i;
	char		keycode[10];
	size_t		cursor;
	char		*carriage_return;
	char		*clear_down;
	char		*clear_till_eol;
	char		*clear_all;
	char		*cursor_move_left;
	char		*cursor_move_right;
	char		*cursor_move_down;
	char		*cursor_move_up;
	char		**child_argv;	
	struct termios	t_original;
	struct termios	t_custom;
	int			custom_terminal;

}				t_shell;

char	**add_element_to_char_array(char **array, char *string);
char	**create_char_array_copy(char **src, int extra);
char	*argument_by_index(t_shell *sh, char *str, int index);
char	*create_kv_string(char *key, char *value);
char	*dir_slash_exec(char *dir, char *cmd);
char	*find_command_path(t_shell *sh, char *cmd);
char	*get_word_by_index(char *str, int index);
char	*kv_array_get_key_value(char **array, char *key);
int		builtin_cmd_index(char *cmd);
int		count_char_array(char **array);
int		count_command_arguments(char *str);
int		ft_isalnum2(int c);
int		get_word_length(char *str);
int		is_alphanumeric_string(char *str);
int		is_fullpath_provided(char *fullpath);
int		kv_array_get_key_index(char **array, char *key);
int		main(int argc, char **argv, char **envp);
int		path_has_executable(char *path, char *cmd);
int		handle_dollar_sign(t_av *av, t_shell *sh, int *i, int *k);
int		handle_strong_quote(t_av *av, int *i, int *k);
int		handle_weak_quote(t_av *av, t_shell *sh, int *i, int *k);
int		unmatched_quote_error(t_av *av, char quote);
int		build_child_argv_list(t_shell *sh, int i, int k, int sub_op);
int		main(int ac, char **av, char **env);
t_shell	*init_shell(int argc, char **argv, char **envp);
void	add_string_to_child_argv(t_shell *sh, char *str, int *k);
void	builtin_cd(t_shell *sh);
void	builtin_echo(t_shell *sh);
void	builtin_env(t_shell *sh);
void	builtin_exit(t_shell *sh);
void	builtin_help(t_shell *sh);
void	builtin_setenv(t_shell *sh);
void	builtin_setenv_kv(t_shell *sh, int arg_count);
void	builtin_unsetenv(t_shell *sh);
void	check_special_keys(t_shell *sh);
void	clean_up(t_shell *sh);
void	cleanup_av_buffers(t_av *av);
void	destroy_char_array(char **array);
void	execute_external_cmd(t_shell *sh);
void	fatal_error(t_shell *sh);
void	fatal_error_message(t_shell *sh, char *msg);
void	fork_exec_wait(t_shell *sh, char *fullpath);
void	get_input(t_shell *sh);
void	increase_buffer(t_shell *sh);
void	key_backspace_function(t_shell *sh);
void	key_ctrl_a_function(t_shell *sh);
void	key_ctrl_e_function(t_shell *sh);
void	key_ctrl_k_function(t_shell *sh);
void	key_ctrl_l_function(t_shell *sh);
void	key_delete_function(t_shell *sh);
void	key_down_arrow_function(t_shell *sh);
void	key_left_arrow_function(t_shell *sh);
void	key_right_arrow_function(t_shell *sh);
void	key_tab_function(t_shell *sh);
void	key_up_arrow_function(t_shell *sh);
void	kv_array_remove_key(char **array, char *key);
void	kv_array_set_key_value(char ***array, char *key, char *value);
void	raw_read(t_shell *sh);
void	reprint_input(t_shell *sh);
void	terminal_init(t_shell *sh);
void	termios_restore_settings(t_shell *sh);
void	termios_save_settings(t_shell *sh);

#endif
