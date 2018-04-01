/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/30 19:51:05 by asarandi          #+#    #+#             */
/*   Updated: 2018/03/31 19:41:21 by asarandi         ###   ########.fr       */
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

#define	shell_prompt	"$> "
#define	shell_name		"minishell"
#define PAGE_SIZE		4096
#define e_nomem			"out of memory"
#define e_readfail		"read() failed"
#define e_gnlfail		"get_next_line() failed"
#define NUM_BUILTINS	7

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
	sh->bufsize++;
	sh->bufsize--;
	//echo, echo -n
}

void	builtin_cd(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//cd <path>, cd, cd ~, cd -
}

void	builtin_setenv(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//setenv
	//setenv: Too many arguments.
	// 0-2 arguments
	//setenv: Variable name must begin with a letter.
	//
}

void	builtin_unsetenv(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//unsetenv
	//unsetenv: Too few arguments.
	//number of arguments should be >= 1
}

void	builtin_env(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//env
	//without argument prints the environment variables
}

void	builtin_exit(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//exit
}

void	builtin_help(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//show help/usage info
}

void	clean_up(t_shell *sh)
{
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
	ft_printf(STDERR_FILENO, "%s: error: %s\n", shell_name, msg);
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
		dst[i] = NULL;
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

t_shell	*init(int argc, char **argv, char **envp)
{
	t_shell	*sh;

	if ((sh = ft_memalloc(sizeof(sh))) == NULL)
		fatal_error_message(sh, e_nomem);
	sh->argc = argc;
	sh->argv = argv;
	sh->envp = envp;
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
	ft_printf(STDOUT_FILENO, "%s", shell_prompt);
	sh->buffer = NULL;
	if (get_next_line(STDIN_FILENO, &(sh->buffer)) == -1)
		fatal_error_message(sh, e_gnlfail);
	return ;
}

int	builtin_cmd_index(char *cmd)
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

char	*get_word_by_index(char *str, int index)
{
	int		i;

	i = 0;
	while ((str[i]) && (ft_isspace(str[i])))
		i++;
	if (str[i] == 0)
		return (NULL);
	if (index == 0)
		return (&str[i]);
	while ((str[i]) && (!ft_isspace(str[i])))
		i++;
	return (get_word_by_index(&str[i], index - 1));
}

int		get_word_length(char *str)
{
	int		i;

	i = 0;
	while ((str[i]) && (!ft_isspace(str[i])))
		i++;
	return (i);
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
		fatal_error_message(sh, e_nomem);
	return (ft_strncpy(result, argument, length));
}

int		main(int argc, char **argv, char **envp)
{
	t_shell	*sh;
	char	*argument;
	int		i;

	sh = init(argc, argv, envp);
	while (1)
	{
		get_input(sh);
		i = 0;
		while ((argument = argument_by_index(sh, sh->buffer, i)) != NULL)
		{
			ft_printf(1, "index %d, argument = %s\n", i, argument);
			free(argument);
			i++;
		}

		if (ft_strcmp(sh->buffer, "exit") == 0)
			break ;
//		execute(sh);
		if (sh->buffer != NULL)
			free(sh->buffer);
	}
	clean_up(sh);
	return (0);
}
