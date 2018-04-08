/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autocomplete.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/08 05:32:42 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/08 06:10:11 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_to_list_of_executables(t_shell *sh, char *cmd)
{
	t_exec	*record;

	record = ft_memalloc(sizeof(t_exec));
	record->next = NULL;
	record->cmd = cmd;
	if (sh->exec == NULL)
		sh->exec = record;
	else
		sh->last_exec->next = record;
	sh->last_exec = record;
}

void	destroy_list_of_executables(t_shell *sh)
{
	t_exec	*ptr;
	t_exec	*ptr_next;

	ptr = sh->exec;
	while (ptr != NULL)
	{
		ptr_next = ptr->next;
		free(ptr->cmd);
		free(ptr);
		ptr = ptr_next;
	}
	sh->exec = NULL;
	sh->last_exec = NULL;
}

void	build_list_of_executables(t_shell *sh)
{
	char	**folders;
	char	*norme;
	DIR		*dirp;
	struct	dirent	*dp;
	int		i;

	if ((norme = kv_array_get_key_value(sh->envp, "PATH")) == NULL)
		return ;
	if ((folders = ft_strsplit(norme, ':')) == NULL)
		fatal_error_message(sh, E_NOMEM);
	i = 0;
	while (folders[i])
	{
		if ((dirp = opendir(folders[i])) != NULL)
		{	
			while ((dp = readdir(dirp)) != NULL)
			{
				norme = dir_slash_exec(folders[i], dp->d_name);
				if (is_valid_executable_file(norme) == 1)
					add_to_list_of_executables(sh, norme);
				else
					free(norme);
			}
			(void)closedir(dirp);
		}
		i++;
	}
	destroy_char_array(folders);
}
