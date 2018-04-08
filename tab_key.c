/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tab_key.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/06 05:25:17 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/08 06:23:19 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*basename(char *str)
{
	char	*result;

	result = ft_strrchr(str, '/');
	if (result == NULL)
		return (EMPTY_STRING);
	else if (result[1] == 0)
		return (EMPTY_STRING);
	return (&result[1]);
}

int		tab_count_matches(t_shell *sh)
{
	int		count;
	int		needle_len;
	t_exec	*ptr;

	if ((needle_len = ft_strlen(sh->buffer)) < 1)
		return (0);
	ptr = sh->exec;
	count = 0;
	while (ptr != NULL)
	{
		if (ft_strncmp(sh->buffer, basename(ptr->cmd), needle_len) == 0)
			count += 1;
		ptr = ptr->next;
	}
	return (count);
}

int		string_has_whitespace(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (ft_isspace(str[i]))
			return (1);
		i++;
	}
	return (0);
}

t_exec	**tab_array_of_matches(t_shell *sh)
{
	t_exec	**result;
	t_exec	*ptr;
	int		count;
	int		i;
	int		needle_len;

	count = tab_count_matches(sh);
	result = ft_memalloc((count + 1) * sizeof(t_exec *));
	i = 0;
	ptr = sh->exec;
	needle_len = ft_strlen(sh->buffer);
	while (i < count)
	{
		if (ft_strncmp(sh->buffer, basename(ptr->cmd), needle_len) == 0)
			result[i++] = ptr;
		ptr = ptr->next;
	}
	result[i] = NULL;
	return (result);
}

void	key_tab_function(t_shell *sh)
{
	t_exec	**matches;
	int		i;

	if ((sh->input_size > 0) && (string_has_whitespace(sh->buffer) == 0))
	{
		matches = tab_array_of_matches(sh);
		i = tab_count_matches(sh);
		if (i == 1)
		{
			ft_strcpy(sh->buffer, basename(matches[0]->cmd));
			sh->buf_i = ft_strlen(basename(matches[0]->cmd));
			sh->buffer[sh->buf_i++] = ' ';
			sh->input_size = sh->buf_i;
		}
		else if (i > 1)
		{
			ft_printf(1, "\n");
			i = 0;
			while (matches[i] != 0)
			{
				ft_printf(1, "%s\n", basename(matches[i]->cmd));
				i++;
			}
		}
		free(matches);
		reprint_input(sh);

		//
		// get window size
		//
	}
	return ;
}
