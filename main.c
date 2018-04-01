/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/30 19:51:05 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/01 06:02:31 by asarandi         ###   ########.fr       */
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

#define	SHELL_PROMPT	"$> "
#define	SHELL_NAME		"minishell"
#define PAGE_SIZE		4096
#define E_NOMEM			"out of memory"
#define E_READFAIL		"read() failed"
#define E_GNLFAIL		"get_next_line() failed"
#define	E_ALNUM			"Variable name must contain alphanumeric characters."
#define E_TOOMANY		"Too many arguments."
#define E_TOOFEW		"Too few arguments."
#define E_LETTER		"Variable name must begin with a letter."


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
	int	dash_n;
	char *word;

	dash_n = 0;
	word = argument_by_index(sh, sh->buffer, 1);
	if (ft_strcmp(word, "-n") == 0)
		dash_n = 1;
	free(word);
	if ((word = get_word_by_index(sh->buffer, 1 + dash_n)) == NULL)
		return ;
	ft_printf(STDOUT_FILENO, "%s", word);
	if (dash_n == 0)
		ft_printf(STDOUT_FILENO, "\n");
}

void	builtin_cd(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//cd <path>, cd, cd ~, cd -
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

void	builtin_setenv_kv(t_shell *sh, int have_value)
{
	char	*key;
	char	*value;

	key = argument_by_index(sh, sh->buffer, 1);
	value  = argument_by_index(sh, sh->buffer, 2);
	if (is_alphanumeric_string(key) == 1)
	{
		if (ft_isalpha(key[0]))
		{
			if (have_value == 1)
				kv_array_set_key_value(&sh->envp, key, value);
			else
				kv_array_set_key_value(&sh->envp, key, "");
		}
		else
			ft_printf(STDERR_FILENO, "setenv: %s\n", E_LETTER);
	}
	else
		ft_printf(STDERR_FILENO, "setenv: %s\n", E_ALNUM);
	free(key);
	if (value != NULL)
		free(value);
}

void	builtin_setenv(t_shell *sh)
{
	int		count;

	count = count_command_arguments(sh->buffer);
	if (count == 1)
		return builtin_env(sh);
	else if (count == 2)
		return builtin_setenv_kv(sh, 0);
	else if (count == 3)
		return builtin_setenv_kv(sh, 1);
	else if (count > 3)
		ft_printf(STDERR_FILENO, "setenv: %s\n", E_TOOMANY);
}

void	builtin_unsetenv(t_shell *sh)
{
	char	*key;
	int		i;
	int		count;


	count = count_command_arguments(sh->buffer);
	i = 1;
	while (i < count)
	{
		key = argument_by_index(sh, sh->buffer, i);
		kv_array_remove_key(sh->envp, key);
		free(key);
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
	ft_printf(STDERR_FILENO, "exit\n");
	clean_up(sh);
	exit(EXIT_SUCCESS);
}

void	builtin_help(t_shell *sh)
{
	sh->bufsize++;
	sh->bufsize--;
	//show help/usage info
}

void	clean_up(t_shell *sh)
{
	if (sh->envp != NULL)
		destroy_char_array(sh->envp);
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
	int	count;
	int i;

	index = kv_array_get_key_index(array, key);
	if (index == -1)
		return ;
	free(array[index]);
	count = count_char_array(array);
	i = index + 1;
	while (array[i] != NULL)
	{
		array[index++] = array[i++];
	}
	array[index] = NULL;
	return ;
}

t_shell	*init(int argc, char **argv, char **envp)
{
	t_shell	*sh;

	if ((sh = ft_memalloc(sizeof(t_shell))) == NULL)
		fatal_error_message(sh, E_NOMEM);
	sh->argc = argc;
	sh->argv = argv;
	sh->envp = create_char_array_copy(envp, 0);
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

void	add_string_to_child_argv(t_shell *sh, char *str)
{
	char **old_array;
	char *new_string;

	old_array = sh->child_argv;
	new_string = ft_strdup(str);
	sh->child_argv = add_element_to_char_array(sh->child_argv, new_string);
	destroy_char_array(old_array);
}



void	build_child_argv_list(t_shell *sh)
{
	int	i;
	int k;
	int j;
	int	flag;
	char	*key_name;
	char	*value;
	char	*str;
	char	*buffer;

	str = sh->buffer;

#define STRONG_Q		0x27
#define WEAK_Q			0x22
#define BACKSLASH		0x5c
#define EMPTY_STRING	""

	buffer = ft_memalloc(PAGE_SIZE * 2);	/* XXX buffer for argument*/
	key_name = ft_memalloc(PAGE_SIZE);	/* XXX */
	i = 0;
	k = 0;
	flag = 0;

	while ((str[i]) && (ft_isspace(str[i])))
		i++;
	while (str[i])
	{
//////////////////////////////////////////////////////////////////////////////////////////
		if (str[i] == STRONG_Q)
		{
			flag = 1;
			i++;
			if (ft_strchr(&str[i], STRONG_Q) == NULL)
			{
				//error, return
				ft_printf(STDERR_FILENO, "Unmatched '.\n");
			}
			while (str[i] != STRONG_Q)
				buffer[k++] = str[i++];
			i++;
			if ((ft_isspace(str[i])) || (str[i] == 0))
			{
				buffer[k] = 0;
				//char	**add_element_to_char_array(char **array, char *string)
				add_string_to_child_argv(sh, buffer);				//done with this argument, get next
				k = 0;
			}
			continue ;
		}
//////////////////////////////////////////////////////////////////////////////////////////
		else if (str[i] == WEAK_Q)
		{
			flag = 2;
			i++;
			if (ft_strchr(&str[i], WEAK_Q) == NULL)
			{
				//error, return
				ft_printf(STDERR_FILENO, "Unmatched \".\n");
			}
			while (str[i] != WEAK_Q)
			{
				if ((str[i] == '$') && (ft_isalpha(str[i + 1])))
				{
					i++;
					j = 0;
					while (ft_isalnum2(str[i + j]))
						j++;
					ft_strncpy(key_name, &str[i], j);
					key_name[j] = 0;
					value = kv_array_get_key_value(sh->envp, key_name);
					if (!value)
					{
						//error	.. no such variable, return
						ft_printf(STDERR_FILENO, "%s: Undefined variable.\n", key_name);
						value = EMPTY_STRING;
						// return
					}
					j = 0;
					while (value[j])
						buffer[k++] = value[j++];
					i += ft_strlen(key_name);
					continue;
				}
				else
					buffer[k++] = str[i++];
			}
			i++;
			if ((ft_isspace(str[i])) || (str[i] == 0))
			{
				buffer[k] = 0;
				//char	**add_element_to_char_array(char **array, char *string)
				add_string_to_child_argv(sh, buffer);				//done with this argument, get next
				k = 0;
			}
			continue ;
		}
	//////////////////////////////////////////////////////////////////////////////////////////
		else if (str[i] == BACKSLASH)
		{
			i++;
			buffer[k++] = str[i++];
		}
	//////////////////////////////////////////////////////////////////////////////////////////


		else if ((str[i] == '$') && (ft_isalpha(str[i + 1])))
		{
			i++;
			j = 0;
			while (ft_isalnum2(str[i + j]))
				j++;
			ft_strncpy(key_name, &str[i], j);
			key_name[j] = 0;
			value = kv_array_get_key_value(sh->envp, key_name);
			if (!value)
			{
				//error	.. no such variable, return
				ft_printf(STDERR_FILENO, "%s: Undefined variable.\n", key_name);
				value = EMPTY_STRING;
				// return
			}
			j = 0;
			while (value[j])
				buffer[k++] = value[j++];
			i += ft_strlen(key_name);
		}

		else
			buffer[k++] = str[i++];

		if ((ft_isspace(str[i])) || (str[i] == 0))
		{
			buffer[k] = 0;
			add_string_to_child_argv(sh, buffer);				//done with this argument, get next
			k = 0;
			while ((str[i]) && (ft_isspace(str[i])))
				i++;
		}
	}
	free(buffer);
	free(key_name);
}

int		main(int argc, char **argv, char **envp)
{
	t_shell	*sh;
	char	*argument;
	int		i, j;
	int		builtin;

	sh = init(argc, argv, envp);
	while (1)
	{
		get_input(sh);
		i = 0;
		sh->child_argv = ft_memalloc(sizeof(char *));
		sh->child_argv[0] = NULL;
		build_child_argv_list(sh);
		j = 0;
		while (sh->child_argv[j])
		{
			ft_printf(1, "child argv[%d] = %s\n", j, sh->child_argv[j]);
			j++;
		}

		argument = argument_by_index(sh, sh->buffer, i);
		if (argument != NULL)
		{
//			ft_printf(1, "index %d, argument = %s\n", i, argument);
			builtin = builtin_cmd_index(argument);
			free(argument);
			if (builtin != -1)
				builtin_functions[builtin](sh);	//run built-in
			else
				builtin = 0;					//run external command
		}

		destroy_char_array(sh->child_argv);

		if (ft_strcmp(sh->buffer, "exit") == 0)
			break ;
//		execute(sh);
		if (sh->buffer != NULL)
			free(sh->buffer);
	}
	clean_up(sh);
	return (0);
}
