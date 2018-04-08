/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/06 05:06:58 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/08 01:34:34 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		builtin_cmd_index(char *cmd)
{
	int			i;
	const char	*builtin_list[] = {
		"echo", "cd", "setenv", "unsetenv", "env", "exit", "help"};

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

char	*builtin_cd_get_kv(t_shell *sh, char *variable)
{
	char	*result;

	result = NULL;
	result = kv_array_get_key_value(sh->envp, variable);
	if (result == NULL)
		(void)ft_printf(STDERR_FILENO, E_NOVARIABLE, variable);
	return (result);	
}

char	*builtin_cd_get_path(t_shell *sh)
{
	char	*tempo;
	char	*path;

	path = NULL;
	tempo = builtin_cd_get_kv(sh, "HOME");
	if ((sh->child_argv[1] == NULL) || (ft_strcmp(sh->child_argv[1], "~") == 0))
	{
		if (tempo != NULL)
			path = ft_strdup(tempo);
	}
	else if (ft_strncmp(sh->child_argv[1], "~/", 2) == 0)
	{
		if (tempo != NULL)
			path = dir_slash_exec(tempo, &sh->child_argv[1][2]);
	}
	else if ((ft_strcmp(sh->child_argv[1], "-")) == 0)
	{
		if ((tempo = builtin_cd_get_kv(sh, "OLDPWD")) != NULL)
			path = ft_strdup(tempo);
	}
	else
		path = ft_strdup(sh->child_argv[1]);
	return (path);
}

void	builtin_cd_save_cwd(t_shell *sh, char *variable)
{
	char	*cwd;

	cwd = NULL;
	cwd = getcwd(NULL, 0);
	if (cwd != NULL)
	{
		kv_array_set_key_value(&sh->envp, variable, cwd);
		free(cwd);
	}
	else
		ft_printf(STDERR_FILENO, "%s\n", E_CWDFAIL);
	return ;
}

void	builtin_cd(t_shell *sh)
{
	char	*path;
	int		r;

	if (count_char_array(sh->child_argv) > 2)
		return ((void)ft_printf(STDERR_FILENO, E_TOOMANY2, "cd"));
	path = builtin_cd_get_path(sh);
	if (path != NULL)
	{
		builtin_cd_save_cwd(sh, "OLDPWD");
		r = chdir(path);
		free(path);
		if (r == -1)
			(void)ft_printf(STDERR_FILENO, E_CHDIRFAIL);
		else
			builtin_cd_save_cwd(sh, "PWD");
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
	return ;
}

void	builtin_help(t_shell *sh)
{
	sh->bufsize += 0;
	return ;
}
