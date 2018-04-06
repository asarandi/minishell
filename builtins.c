/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/06 05:06:58 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/06 06:27:15 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
