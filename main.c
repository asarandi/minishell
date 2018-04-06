
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/30 19:51:05 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/04 20:11:18 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
typedef struct	s_shell
{
	int			argc;
	char		**argv;
	char		**envp;
	char		*buffer;
	size_t		bufsize;
}				t_shell;
*/

#define	SHELL_PROMPT		"$> "
#define	SHELL_NAME			"minishell"
#define PAGE_SIZE			4096
#define E_NOMEM				"out of memory"
#define E_READFAIL			"read() failed"
#define E_GNLFAIL			"get_next_line() failed"
#define E_CWDFAIL			"getcwd() failed"
#define	E_NOVARIABLE		"No %s variable set.\n"
#define E_CHDIRFAIL			"chdir() failed\n"
#define	E_ALNUM				"Variable name must contain alphanumeric characters."
#define E_TOOMANY			"Too many arguments."
#define E_TOOMANY2			"%s: Too many arguments.\n"
#define E_TOOFEW			"Too few arguments."
#define E_LETTER			"Variable name must begin with a letter."
#define	E_NOTFOUND			"%s: Command not found.\n"
#define E_FORK				"fork() failed"
#define UNDEFINED_VARIABLE	"%s: Undefined variable.\n"
#define UNMATCHED_QUOTE		"Unmatched %c.\n"
#define STRONG_QUOTE		0x27
#define WEAK_QUOTE			0x22
#define BACKSLASH			0x5c
#define EMPTY_STRING		""
#define DOLLAR_SIGN			'$'
#define NUM_BUILTINS		7
#define	HISTORY_FILE		".minishell_history"
#define	KEY_UP_ARROW		"\e[A"
#define KEY_DOWN_ARROW		"\e[B"
#define	KEY_LEFT_ARROW		"\e[D"
#define	KEY_RIGHT_ARROW		"\e[C"
#define KEY_BACKSPACE		"\x7f"
#define KEY_DELETE			"\e[3~"
#define KEY_CTRL_A			"\x01"
#define KEY_CTRL_E			"\x05"
#define KEY_CTRL_K			"\x0b"
#define KEY_CTRL_L			"\x0c"
#define KEY_TAB				"\x09"


const char *special_keys[] = {
		KEY_UP_ARROW,
		KEY_DOWN_ARROW,
		KEY_LEFT_ARROW,
		KEY_RIGHT_ARROW,
		KEY_BACKSPACE,
		KEY_DELETE,
		KEY_CTRL_A,
		KEY_CTRL_E,
		KEY_CTRL_K,
		KEY_CTRL_L,
		KEY_TAB
};

void	(*special_key_functions[]) (t_shell *) = 
{
	&key_up_arrow_function,
	&key_down_arrow_function,
	&key_left_arrow_function,
	&key_right_arrow_function,
	&key_backspace_function,
	&key_delete_function,
	&key_ctrl_a_function,
	&key_ctrl_e_function,
	&key_ctrl_k_function,
	&key_ctrl_l_function,
	&key_tab_function
};

const char *builtin_list[] = {
	"echo", "cd", "setenv", "unsetenv", "env", "exit", "help"};

void (*builtin_functions[]) (t_shell *) = {
	&builtin_echo,
	&builtin_cd,
	&builtin_setenv,
	&builtin_unsetenv,
	&builtin_env,
	&builtin_exit,
	&builtin_help
};

void	builtin_echo(t_shell *sh)
{
	int	dash_n;
	int	i;

	dash_n = 0;
	if (sh->child_argv[1] != NULL)
	{
		if (ft_strcmp(sh->child_argv[1], "-n") == 0)
			dash_n = 1;
		i = 1 + dash_n;
		while (sh->child_argv[i] != NULL)
		{
			ft_printf(STDOUT_FILENO, "%s", sh->child_argv[i]);
			if (sh->child_argv[++i] != NULL)
				ft_printf(STDOUT_FILENO, " ");
		}
	}
	if (dash_n == 0)
		ft_printf(STDOUT_FILENO, "\n");
}

void	builtin_cd(t_shell *sh)
{
	char	*cwd;
	char	*path;

	if ((sh->child_argv[1] != NULL) && (sh->child_argv[2] != NULL))		//too many arguments
		return ((void)ft_printf(STDERR_FILENO, E_TOOMANY2, "cd"));
	if ((sh->child_argv[1] == NULL) || (ft_strcmp(sh->child_argv[1], "~") == 0))	//[cd] or [cd ~]
	{
		if ((path = kv_array_get_key_value(sh->envp, "HOME")) == NULL)
			return ((void)ft_printf(STDERR_FILENO, E_NOVARIABLE, "$HOME"));
	}
	else if ((ft_strcmp(sh->child_argv[1], "-")) == 0)
	{
		if ((path = kv_array_get_key_value(sh->envp, "OLDPWD")) == NULL)
			return ((void)ft_printf(STDERR_FILENO, E_NOVARIABLE, "$OLDPWD"));
	}
	else
		path = sh->child_argv[1];
	cwd = NULL;
	cwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
	{
		if (cwd != NULL)
			free(cwd);
		return ((void)ft_printf(STDERR_FILENO, E_CHDIRFAIL));
	}
	if (cwd != NULL)
	{
		kv_array_set_key_value(&sh->envp, "OLDPWD", cwd);
		free(cwd);
	}
	if ((cwd = getcwd(NULL, 0)) == NULL)
		ft_printf(STDERR_FILENO, "%s\n", E_CWDFAIL);
	else
	{
		kv_array_set_key_value(&sh->envp, "PWD", cwd);
		free(cwd);
	}
}

//setenv: Variable name must contain alphanumeric characters.

int		ft_isalnum2(int c)
{
	if ((c >= '0') && (c <= '9'))
		return (1);
	else if ((c >= 'A') && (c <= 'Z'))
		return (1);
	else if ((c >= 'a') && (c <= 'z'))
		return (1);
	else if (c == '_')
		return (1);
	else
		return (0);
}

int		is_alphanumeric_string(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!ft_isalnum2(str[i]))
			return (0);
		i++;
	}
	return (1);
}

void	builtin_setenv_kv(t_shell *sh, int arg_count)
{
	char	*key;
	char	*value;

	key = sh->child_argv[1];
	value = EMPTY_STRING;
	if (arg_count == 3)
		value  = sh->child_argv[2];
	if (is_alphanumeric_string(key) == 1)
	{
		if (ft_isalpha(key[0]))
			kv_array_set_key_value(&sh->envp, key, value);
		else
			ft_printf(STDERR_FILENO, "setenv: %s\n", E_LETTER);
	}
	else
		ft_printf(STDERR_FILENO, "setenv: %s\n", E_ALNUM);
}

void	builtin_setenv(t_shell *sh)
{
	int		count;

	count = count_char_array(sh->child_argv);
	if (count == 1)
		return builtin_env(sh);
	else if (count == 2)
		return builtin_setenv_kv(sh, 2);
	else if (count == 3)
		return builtin_setenv_kv(sh, 3);
	else if (count > 3)
		ft_printf(STDERR_FILENO, "setenv: %s\n", E_TOOMANY);
}

void	builtin_unsetenv(t_shell *sh)
{
	char	*key;
	int		i;
	int		count;

	count = count_char_array(sh->child_argv);
	i = 1;
	while (i < count)
	{
		key = sh->child_argv[i];
		kv_array_remove_key(sh->envp, key);
		i++;
	}
	if (i == 1)
		ft_printf(STDERR_FILENO, "unsetenv: %s\n", E_TOOFEW);
}

void	builtin_env(t_shell *sh)
{
	int	i;

	i = 0;
	while (sh->envp[i] != NULL)
	{
		ft_printf(STDOUT_FILENO, "%s\n", sh->envp[i]);
		i++;
	}
	return ;
}

void	builtin_exit(t_shell *sh)
{
	int	exit_code;

	exit_code = EXIT_SUCCESS;
	if (sh->child_argv[1] != NULL)
		exit_code = ft_atoi(sh->child_argv[1]);
	ft_printf(STDERR_FILENO, "exit\n");
	clean_up(sh);
	exit(exit_code);
}

void	builtin_help(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//show help/usage info
}

void	clean_up(t_shell *sh)
{
	termios_restore_settings(sh);
	if (sh->envp != NULL)
		destroy_char_array(sh->envp);
	if (sh->history != NULL)
		destroy_char_array(sh->history);
	if (sh->child_argv != NULL)
		destroy_char_array(sh->child_argv);
	if (sh->buffer != NULL)
		free(sh->buffer);
	if (sh != NULL)
		free(sh);
}

void	fatal_error(t_shell *sh)
{
	clean_up(sh);
	exit(EXIT_FAILURE);
}

void	fatal_error_message(t_shell *sh, char *msg)
{
	ft_printf(STDERR_FILENO, "%s: error: %s\n", SHELL_NAME, msg);
	fatal_error(sh);
}

int		count_char_array(char **array)
{
	int	i;

	i = 0;
	while (array[i] != NULL)
		i++;
	return (i);
}

char	**create_char_array_copy(char **src, int extra)
{
	char	**dst;
	int		count;
	int		i;

	count = count_char_array(src);
	dst = ft_memalloc((count + 1 + extra) * sizeof(char *));
	i = 0;
	while (i < count)
	{
		dst[i] = ft_strdup(src[i]);
		i++;
	}
	while (i < count + 1 + extra)
		dst[i++] = NULL;
	return (dst);
}

void	destroy_char_array(char **array)
{
	int	count;
	int	i;

	count = count_char_array(array);
	i = 0;
	while (i < count)
	{
		free(array[i]);
		i++;
	}
	free(array);
	return ;
}


//
// this will create an array copy and add [string] as last element
// old array is not destroyed, [string] is not duplicated
//
char	**add_element_to_char_array(char **array, char *string)
{
	char	**new_array;
	int		count;

	new_array = create_char_array_copy(array, 1);
	count = count_char_array(new_array);
	new_array[count] = string;
	return (new_array);
}

int		kv_array_get_key_index(char **array, char *key)
{
	int	count;
	int klen;
	int i;

	count = count_char_array(array);
	klen = ft_strlen(key);
	i = 0;
	while (i < count)
	{
		if (ft_strncmp(array[i], key, klen) == 0)
		{
			if (array[i][klen] == '=')
				return (i);
		}
		i++;
	}
	return (-1);
}

char	*kv_array_get_key_value(char **array, char *key)
{
	int	index;
	char *value;

	index = kv_array_get_key_index(array, key);
	if (index == -1)
		return (NULL);
	value = ft_strchr(array[index], '=');
	if (value == NULL)							//should not happen
		return (NULL);
	return (value + 1);
}

char	*create_kv_string(char *key, char *value)
{
	char	*result;
	int		k_len;
	int		v_len;

	k_len = ft_strlen(key);
	v_len = ft_strlen(value);	
	result = ft_memalloc(k_len + v_len + 2);
	ft_strncpy(result, key, k_len);
	result[k_len] = '=';
	ft_strncpy(&result[k_len + 1], value, v_len);
	return (result);
}

void	kv_array_set_key_value(char ***array, char *key, char *value)
{
	int		index;
	char	**new_array;
	char	*new_element;

	index = kv_array_get_key_index((*array), key);
	new_element = create_kv_string(key, value);
	if (index == -1)
	{
		new_array = add_element_to_char_array((*array), new_element);
		destroy_char_array((*array));
		*array = new_array;
		return ;
	}
	else
	{
		free((*array)[index]);
		(*array)[index] = new_element;
	}
	return ;
}

void	kv_array_remove_key(char **array, char *key)
{
	int	index;
	int i;

	index = kv_array_get_key_index(array, key);
	if (index == -1)
		return ;
	free(array[index]);
	i = index + 1;
	while (array[i] != NULL)
	{
		array[index++] = array[i++];
	}
	array[index] = NULL;
	return ;
}

t_shell	*init_shell(int argc, char **argv, char **envp)
{
	t_shell	*sh;

	if ((sh = ft_memalloc(sizeof(t_shell))) == NULL)
		fatal_error_message(sh, E_NOMEM);
	sh->argc = argc;
	sh->argv = argv;
	sh->envp = create_char_array_copy(envp, 0);
	terminal_init(sh);
	return (sh);
}

/*
void	increase_buffer(t_shell *sh)
{
	char	*newbuf;

	if ((newbuf = ft_memalloc(sh->bufsize + PAGE_SIZE)) == NULL)
		fatal_error_message(sh, e_nomem);
	ft_strncpy(newbuf, sh->buffer, sh->bufsize);
	free(sh->buffer);
	sh->buffer = newbuf;
	sh->bufsize += PAGE_SIZE;
}
*/

/*
void	get_input(t_shell *sh)
{
	size_t	i;
	ssize_t	r;

	ft_printf(STDOUT_FILENO, "%s", shell_prompt);
	if ((sh->buffer = ft_memalloc(PAGE_SIZE)) == NULL)
		fatal_error_message(sh, e_nomem);
	sh->bufsize = PAGE_SIZE;
	i = 0;
	while (1)
	{
		if ((r = read(STDIN_FILENO, &(sh->buffer)[i], 1)) == 0)
			break ;
		if (r == -1)
			fatal_error_message(sh, e_readfail);
		if (((sh->buffer)[i] == '\n') || ((sh->buffer)[i] == 0))
			break ;
		i++;
		if (i >= sh->bufsize)
			increase_buffer(sh);
	}
	return ;
}
*/

void	get_input(t_shell *sh)
{
	ft_printf(STDOUT_FILENO, "%s", SHELL_PROMPT);
	sh->buffer = NULL;
	if (get_next_line(STDIN_FILENO, &(sh->buffer)) == -1)
		fatal_error_message(sh, E_GNLFAIL);
	return ;
}

int		builtin_cmd_index(char *cmd)
{
	int	i;

	i = 0;
	while (i < NUM_BUILTINS)
	{
		if (ft_strcmp(cmd, builtin_list[i]) == 0)
			return (i);
		i++;
	}
	return (-1);
}

int		get_word_length(char *str)
{
	int		i;

	i = 0;
	while ((str[i]) && (!ft_isspace(str[i])))
		i++;
	return (i);
}

char	*get_word_by_index(char *str, int index)
{
	int		i;
	int		k;

	i = 0;
	while ((str[i]) && (ft_isspace(str[i])))
		i++;
	if (str[i] == 0)
		return (NULL);
	k = get_word_length(&str[i]);
	if (k == -1)
		return (NULL);
	if (index == 0)
		return (&str[i]);
	i += k;
	return (get_word_by_index(&str[i], index - 1));
}


char	*argument_by_index(t_shell *sh, char *str, int index)
{
	char	*argument;
	int		length;
	char	*result;

	if ((argument = get_word_by_index(str, index)) == NULL)
		return (NULL);
	if ((length = get_word_length(argument)) == 0)
		return (NULL);
	if ((result = ft_memalloc(length + 1)) == NULL)
		fatal_error_message(sh, E_NOMEM);
	return (ft_strncpy(result, argument, length));
}

int		count_command_arguments(char *str)
{
	int		i;
	char	*word;

	i = 0;
	while ((word = get_word_by_index(str, i)) != NULL)
		i++;
	return (i);
}



/*
 * if we have white space at the beginning of the string, we ignore it
 * if we have a:
 * 		single quote - we look for the matching end quote,
 * 		if the character before the opening quote was whitespace,
 * 		then opening quote denotes beginning of string [   'hello '   ] => only [hello ] is copied
 * 		if after closing quote next character is whitespace,
 * 		then that denotes the end of the argument;
 *
 * 		however if before the opening quote and/or after the closing quote
 * 		we have a character that is not whitespace then
 * 		those adjacent characters become part of the argument; [   asd'helo  'zxc ] => [asdhelo  zxc]
 *
 * 		inside single quotes we DO NOT expand shell variables [echo '$HOME'] will result in [$HOME]
 *
 * 		inside doubles quotes we DO expand shell variables [echo asd"$HOME"zxc] will result in [asd/home/user/zxc]
 *
 */

void	cleanup_av_buffers(t_av *av)
{
	if (av->out != NULL)
		free(av->out);
	if (av->key != NULL)
		free(av->key);
	free(av);
	return ;
}

t_av	*init_av_buffers(t_shell *sh)
{
	t_av	*av;

	if ((av = ft_memalloc(sizeof(t_av))) == NULL)
		return (NULL);
	av->in = sh->buffer;
	if ((av->out = ft_memalloc(PAGE_SIZE * 2)) == NULL)	/* XXX buffer for argument*/
	{
		cleanup_av_buffers(av);
		return (NULL);
	}
	if ((av->key = ft_memalloc(PAGE_SIZE)) == NULL)	/* XXX */
	{
		cleanup_av_buffers(av);
		return (NULL);
	}
	av->val = NULL;
	return (av);
}


int	unmatched_quote_error(t_av *av, char quote)
{
	cleanup_av_buffers(av);
	ft_printf(STDERR_FILENO, UNMATCHED_QUOTE, quote);
	return (0);
}

int	handle_strong_quote(t_av *av, int *i, int *k)
{
	char	*in;
	char	*out;

	in = av->in;
	out = av->out;
	(*i)++;
	if ((ft_strchr(&in[*i], STRONG_QUOTE)) == NULL)
		return (unmatched_quote_error(av, STRONG_QUOTE));
	while (in[*i] != STRONG_QUOTE)
		out[(*k)++] = in[(*i)++];
	(*i)++;
	return (1);
}

int	handle_dollar_sign(t_av *av, t_shell *sh, int *i, int *k)
{
	int		z;

	(*i)++;
	z = 0;
	while (ft_isalnum2(av->in[*i + z]))
		z++;
	ft_strncpy(av->key, &(av->in[*i]), z);
	av->key[z] = 0;
	av->val = kv_array_get_key_value(sh->envp, av->key);
	if (av->val == NULL)
	{
		ft_printf(STDERR_FILENO, UNDEFINED_VARIABLE, av->key);
		cleanup_av_buffers(av);
		return (0);
	}
	z = 0;
	while (av->val[z])
		av->out[(*k)++] = av->val[z++];
	(*i) += ft_strlen(av->key);
	return (1);
}

int	handle_weak_quote(t_av *av, t_shell *sh, int *i, int *k)
{
	char	*in;
	char	*out;

	in = av->in;
	out = av->out;
	(*i)++;
	if ((ft_strchr(&av->in[*i], WEAK_QUOTE)) == NULL)
		return (unmatched_quote_error(av, WEAK_QUOTE));
	while (av->in[*i] != WEAK_QUOTE)
	{
		if ((av->in[*i] == DOLLAR_SIGN) && (ft_isalpha(av->in[*i + 1])))
			(void)handle_dollar_sign(av, sh, i, k);
		else
			out[(*k)++] = in[(*i)++];
	}
	(*i)++;
	return (1);
}

void	add_string_to_child_argv(t_shell *sh, char *str, int *k)
{
	char **old_array;
	char *new_string;

	if ((str == NULL) || (str[0] == 0))
		return ;
	str[*k] = 0;
	*k = 0;
	old_array = sh->child_argv;
	new_string = ft_strdup(str);
	sh->child_argv = add_element_to_char_array(sh->child_argv, new_string);
	destroy_char_array(old_array);
}

int build_child_argv_list(t_shell *sh, int i, int k, int sub_op)
{
	t_av	*av;

	if ((av = init_av_buffers(sh)) == NULL)
		return (0);
	while ((av->in[i]) && (ft_isspace(av->in[i])))
		i++;
	while (av->in[i])
	{
		if (av->in[i] == STRONG_QUOTE)
			sub_op = handle_strong_quote(av, &i, &k);
		else if (av->in[i] == WEAK_QUOTE)
			sub_op = handle_weak_quote(av, sh, &i, &k);
		else if ((av->in[i] == DOLLAR_SIGN) && (ft_isalpha(av->in[i + 1])))
			sub_op = handle_dollar_sign(av, sh, &i, &k);
		else if ((av->in[i] == BACKSLASH) && (i++))
			av->out[k++] = av->in[i++];
		else
			av->out[k++] = av->in[i++];
		if (sub_op == 0)
			return (0);
		if ((ft_isspace(av->in[i])) || (av->in[i] == 0))
			add_string_to_child_argv(sh, av->out, &k);
		while ((av->in[i]) && (ft_isspace(av->in[i])))
			i++;
	}
	cleanup_av_buffers(av);
	return (1);
}

char	*dir_slash_exec(char *dir, char *cmd)
{
	char	*result;
	char	*folder;

	if (dir == NULL)
		dir = EMPTY_STRING;
	if ((ft_strlen(dir) > 0) && (dir[ft_strlen(dir) - 1] != '/'))
		folder = ft_strjoin(dir, "/");
	else
		folder = ft_strdup(dir);
	result = ft_strjoin(folder, cmd);
	free(folder);
	return (result);
}
int		path_has_executable(char *path, char *cmd)
{
	char	*fullpath;
	int		result;

	fullpath = dir_slash_exec(path, cmd);
	result = 0;
	if (access(fullpath, F_OK | X_OK) == 0)
		result = 1;
	free(fullpath);
	return (result);
}

char	*find_command_path(t_shell *sh, char *cmd)
{
	char	**folders;
	char	*path;
	char	*result;
	int		i;

	if ((path = kv_array_get_key_value(sh->envp, "PATH")) == NULL)
		return (NULL);
	if ((folders = ft_strsplit(path, ':')) == NULL)
		fatal_error_message(sh, E_NOMEM);
	i = 0;
	result = NULL;
	while (folders[i])
	{
		if (path_has_executable(folders[i], cmd) == 1)
		{
			result = ft_strdup(folders[i]);
			break ;
		}
		i++;
	}
	destroy_char_array(folders);
	return (result);
}


void	fork_exec_wait(t_shell *sh, char *fullpath)
{
	pid_t	child;

	if ((child = fork()) == -1)
		fatal_error_message(sh, E_FORK);
	else if (child == 0)
		execve(fullpath, sh->child_argv, sh->envp);
	waitpid(child, NULL, 0);
}

int		is_fullpath_provided(char *fullpath)
{
	if (access(fullpath, F_OK | X_OK) == 0)
		return (1);
	return (0);
}

void	execute_external_cmd(t_shell *sh)
{
	char	*path;
	char	*fullpath;

	if (is_fullpath_provided(sh->child_argv[0]) == 1)
		fork_exec_wait(sh, sh->child_argv[0]);
	else
	{
		if ((path = find_command_path(sh, sh->child_argv[0])) == NULL)
			return ((void)ft_printf(STDERR_FILENO, E_NOTFOUND, sh->child_argv[0]));
		fullpath = dir_slash_exec(path, sh->child_argv[0]);
		fork_exec_wait(sh, fullpath);
		free(path);
		free(fullpath);
	}
}

void	termios_save_settings(t_shell *sh)
{
	if (tcgetattr(STDIN_FILENO, &sh->t_original) == -1)
		return (fatal_error_message(sh, "tcgetattr() failed"));	
	sh->t_custom = sh->t_original;
	sh->t_custom.c_cc[VMIN] = 1;
	sh->t_custom.c_cc[VTIME] = 0;
	sh->t_custom.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &sh->t_custom) == -1)
		return (fatal_error_message(sh, "tcsetattr() failed"));
	sh->custom_terminal = 1;
	return ;
}

void	termios_restore_settings(t_shell *sh)
{
	if (sh->custom_terminal == 1)
		tcsetattr(STDIN_FILENO, TCSANOW, &sh->t_original);
	return ;
}

void	increase_buffer(t_shell *sh)
{
	char	*tmp;

	tmp	= ft_memalloc(sh->bufsize + PAGE_SIZE);
	(void)ft_strncpy(tmp, sh->buffer, sh->bufsize);
	free(sh->buffer);
	sh->buffer = tmp;
	sh->bufsize += PAGE_SIZE;
	return ;
}

void	terminal_init(t_shell *sh)
{
	char	buf[1024];
	char	*term;

	term = kv_array_get_key_value(sh->envp, "TERM");
	if (term == NULL)
		term = "xterm";
	tgetent(buf, term);
	sh->carriage_return = tgetstr("cr", NULL);
	sh->cursor_move_left = tgetstr("le", NULL);
	sh->cursor_move_right = tgetstr("nd", NULL);
	sh->cursor_move_down = tgetstr("do", NULL);
	sh->cursor_move_up = tgetstr("up", NULL);
	sh->clear_till_eol = tgetstr("ce", NULL);
	sh->clear_all = tgetstr("cl", NULL);
	sh->clear_down = tgetstr("cd", NULL);
	(void)termios_save_settings(sh);
	return ;
}

void	key_left_arrow_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "left arrow key\n");
	if (sh->buf_i > 0)
	{
		sh->buf_i--;
		ft_putstr(sh->cursor_move_left);
	}
}

void	key_right_arrow_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "right arrow key\n");
	if (sh->buf_i < sh->input_size)
	{
		sh->buf_i++;
		ft_putstr(sh->cursor_move_right);
	}
}

void	key_backspace_function(t_shell *sh)
{
	size_t	tempo;
//	ft_printf(STDOUT_FILENO, "backspace key\n");
	if (sh->buf_i > 0)
	{
		key_left_arrow_function(sh);
		tempo = sh->buf_i;
		while (tempo < sh->input_size)
		{
			sh->buffer[tempo] = sh->buffer[tempo + 1];
			tempo++;
		}
		sh->buffer[tempo] = 0;
		sh->input_size--;
		reprint_input(sh);
	}
}

void	key_delete_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "delete key\n");
//	sh->buf_i += 0;			

	size_t	tempo;
//	ft_printf(STDOUT_FILENO, "backspace key\n");
	if (sh->input_size > sh->buf_i)
	{
//		key_left_arrow_function(sh);
		tempo = sh->buf_i;
		while (tempo < sh->input_size)
		{
			sh->buffer[tempo] = sh->buffer[tempo + 1];
			tempo++;
		}
		sh->buffer[tempo] = 0;
		sh->input_size--;
		reprint_input(sh);
	}


}

void	key_ctrl_a_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "ctrl + a\n");
	sh->buf_i = 0;
	reprint_input(sh);
}

void	key_ctrl_e_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "ctrl + e\n");
	sh->buf_i = sh->input_size;
	reprint_input(sh);
}

void	key_ctrl_k_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "ctrl + k\n");
//	sh->buf_i += 0;
	sh->input_size = sh->buf_i;
	ft_bzero(&sh->buffer[sh->buf_i], sh->bufsize - sh->buf_i);
	ft_putstr(sh->clear_till_eol);
	reprint_input(sh);
}

void	key_ctrl_l_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "ctrl + l\n");
//	sh->buf_i += 0;
	ft_putstr(sh->clear_all);
	reprint_input(sh);
}

void	key_tab_function(t_shell *sh)
{
	//when one match only, autocomplete (replace) current word with match and append space,
	//when multiple matches show a list of matches in columns, like ls,
	//when over a hundred matches, prompt user for display, (calculate number of output lines)
	//
	//if first word of command then suggest executables from $PATH folders
	//if not first word, suggest files
	//
	//autocomplete
	//
//	ft_printf(STDOUT_FILENO, "tab\n");
	sh->buf_i += 0;
}

void	check_special_keys(t_shell *sh)
{
	int			size;
	int			i;
	const char	*key;
	int			len;

	i = 0;
	size = sizeof(special_keys) / sizeof(char *);
	while (i < size)
	{
		key = special_keys[i];
		len = ft_strlen(key);
		if (ft_strncmp(key, sh->keycode, len) == 0)
		{
			(void)ft_bzero(sh->keycode, 9);
			return (special_key_functions[i](sh));
		}
		i++;
	}
	return ;
}

void	display_shell_prompt()
{
	ft_printf(STDOUT_FILENO, "%s", SHELL_PROMPT);
}

void	reprint_input(t_shell *sh)
{
	int	move_left;

//	write(1, sh->carriage_return, ft_strlen(sh->carriage_return));
	ft_putstr(sh->carriage_return);
	ft_putstr(sh->clear_down);
	display_shell_prompt();
	ft_printf(STDOUT_FILENO, sh->buffer);
	move_left = ft_strlen(sh->buffer) - sh->buf_i;
	while (move_left--)
		ft_putstr(sh->cursor_move_left);
}

void	insert_key_into_buffer(t_shell *sh)
{
	//insert character into sh->buffer at position sh->buf_i
	//if necessary shift buffer to right
	//move cursor to the left,
	//print shell prompt
	//print contents of buffer
	size_t	size;

	if (sh->buffer[sh->buf_i] != 0)
	{
		size = sh->input_size;
//		ft_printf(1, "size = %zu, buf_i = %zu\n", size, sh->buf_i);
		while (size > sh->buf_i)
		{
			sh->buffer[size] = sh->buffer[size - 1];
			size--;
		}
	}
	sh->buffer[sh->buf_i++] = sh->keycode[0];
	sh->input_size++;
	reprint_input(sh);

}


char	*file_get_contents(char *filename)
{
	int			fd;
	struct stat	st;
	char		*data;

	if ((fd = open(filename, O_RDONLY)) == -1)
		return (NULL);
	data = NULL;
	if ((fstat(fd, &st)) == 0)
	{
		if ((data = ft_memalloc(st.st_size + 1)) == NULL)
		{
			(void)close(fd);
			return (NULL);
		}
		if ((read(fd, data, st.st_size)) != st.st_size)
		{
			free(data);
			data = NULL;
		}
	}
	(void)close(fd);
	return (data);
}

char	*history_file_name(t_shell *sh)
{
	char	*home;
	char	*fullpath;

	home = kv_array_get_key_value(sh->envp, "HOME");
	if (home == NULL)
		home = EMPTY_STRING;
	fullpath = dir_slash_exec(home, HISTORY_FILE);
	return (fullpath);
}

char	**history_array(t_shell *sh)
{
	char	*filename;
	char	*data;
	char	**array;

	filename = history_file_name(sh);
	data = file_get_contents(filename);
	free(filename);
	if (data == NULL)
		return (NULL);
	array = ft_strsplit(data, '\n');
	free(data);
	return (array);
}


void	key_up_arrow_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "[up arrow key]");
//	sh->buf_i += 0;

	char	*result;
	int		k;

	if (sh->history == NULL)
		return ;
	if ((sh->history_i == 0) && (sh->partial_input == NULL))
		sh->partial_input = ft_strdup(sh->buffer);
	if (sh->history_i < sh->history_count)
		sh->history_i++;
	k = sh->history_count - sh->history_i;
	result = sh->history[k];
	if (ft_strlen(result) < sh->bufsize)
	{
		ft_bzero(sh->buffer, sh->bufsize);
		(void)ft_strcpy(sh->buffer, result);
		sh->buf_i = ft_strlen(sh->buffer);
		sh->input_size = sh->buf_i;
		reprint_input(sh);
	}
	return ;
}

void	restore_partial_input(t_shell *sh)
{
	ft_bzero(sh->buffer, sh->bufsize);
	(void)ft_strcpy(sh->buffer, sh->partial_input);
	free(sh->partial_input);
	sh->partial_input = NULL;
	sh->buf_i = ft_strlen(sh->buffer);
	sh->input_size = sh->buf_i;
	reprint_input(sh);
}


void	key_down_arrow_function(t_shell *sh)
{
//	ft_printf(STDOUT_FILENO, "[down arrow key]");
//	sh->buf_i += 0;

	char	*result;
	int		k;

	if (sh->history == NULL)
		return ;
	if (sh->history_i > 0)
		sh->history_i--;
	else
		return ;
	if (sh->history_i == 0)
		return restore_partial_input(sh);
	k = sh->history_count - sh->history_i;
	result = sh->history[k];
	if (ft_strlen(result) < sh->bufsize)
	{
		ft_bzero(sh->buffer, sh->bufsize);
		(void)ft_strcpy(sh->buffer, result);
		sh->buf_i = ft_strlen(sh->buffer);
		sh->input_size = sh->buf_i;
		reprint_input(sh);
	}
	return ;
}

char	*history_get_item(t_shell *sh, int index)
{
	char	**history;
	int		count;
	char	*result;

	if ((history = history_array(sh)) == NULL)
		return (NULL);
	count = count_char_array(history);
	result = NULL;
	if (index <= count)
	{
		result = ft_strdup(history[count - index]);
	}
	destroy_char_array(history);
	return (result);
}

void	history_append_to_file(t_shell *sh)
{
	int		fd;
	char	*filename;

	filename = history_file_name(sh);
	fd = open(filename, O_APPEND | O_CREAT | O_WRONLY, 0644);
	free(filename);
	if (fd == -1)
		return ;
	(void)lseek(fd, 0, SEEK_END);
	(void)write(fd, sh->buffer, sh->input_size);
	(void)write(fd, "\n", 1);
	(void)close(fd);
	return ;
}

void	end_of_input(t_shell *sh)
{
	if (ft_strlen(sh->buffer) > 0)
		(void)history_append_to_file(sh);
	(void)ft_printf(STDOUT_FILENO,  "\n");
	return ;
}

void	init_input_buffer(t_shell *sh)
{
	sh->buffer = ft_memalloc(PAGE_SIZE);	/* XXX */
	sh->bufsize = PAGE_SIZE;
	sh->buf_i = 0;
	sh->input_size = 0;
	sh->history = history_array(sh);
	sh->history_count = 0;
	if (sh->history != NULL)
		sh->history_count = count_char_array(sh->history);
	sh->history_i = 0;
	sh->partial_input = NULL;
	display_shell_prompt();
}


void	raw_read(t_shell *sh)
{
	init_input_buffer(sh);
	while(1)
	{
		if (read(STDIN_FILENO, &sh->keycode, 9) < 0)
			break ;
		if (ft_isprint(sh->keycode[0]))
			insert_key_into_buffer(sh);
		else if (sh->keycode[0] == '\n')
			return end_of_input(sh);
		else
			check_special_keys(sh);
		if (sh->input_size >= sh->bufsize)
			(void)increase_buffer(sh);
	}
	return ;
}

void	clear_input_buffers(t_shell *sh)
{
	if (sh->partial_input != NULL)
		free(sh->partial_input);
	if (sh->history != NULL)
		destroy_char_array(sh->history);
	sh->history = NULL;
	destroy_char_array(sh->child_argv);
	sh->child_argv = NULL;
	free(sh->buffer);
	sh->buffer = NULL;
	return ;
}

int		main(int argc, char **argv, char **envp)
{
	t_shell	*sh;
	int		r;

	sh = init_shell(argc, argv, envp);
	while (1)
	{
		raw_read(sh);
		if (sh->buffer == NULL)
			break ;
		sh->child_argv = ft_memalloc(sizeof(char *));
		sh->child_argv[0] = NULL;
		if ((build_child_argv_list(sh, 0, 0, 1) == 1) && (sh->child_argv[0] != NULL))
		{
			if ((r = builtin_cmd_index(sh->child_argv[0])) != -1)
				(void)builtin_functions[r](sh);
			else
				(void)execute_external_cmd(sh);
		}
		clear_input_buffers(sh);
	}
	clean_up(sh);
	return (0);
}
