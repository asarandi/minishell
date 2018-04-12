/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/08 15:46:44 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/08 15:48:08 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
