/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tab_key.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/06 05:25:17 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/08 10:00:11 by asarandi         ###   ########.fr       */
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


int		tab_max_length(t_exec **array)
{
	int		i;
	int		len;
	int		max_length;

	i = 0;
	max_length = 0;
	while (array[i] != NULL)
	{
		len = ft_strlen(basename(array[i]->cmd));
		if (len > max_length)
			max_length = len;
		i++;
	}
	return (max_length);
}


#define	TAB_SIZE		8



void	tab_bubble_sort(t_exec **a, int count)
{
	int		i;
	int		flag;
	t_exec	*tmp;

	while (1)
	{
		i = 0;
		flag = 0;
		while (i < count - 1)
		{
			if (ft_strcmp(basename(a[i]->cmd), basename(a[i + 1]->cmd)) > 0)
			{
				flag = 1;
				tmp = a[i];
				a[i] = a[i + 1];
				a[i + 1] = tmp;
			}
			i++;
		}
		if (flag == 0)
			break ;
	}
}

void	tab_remove_duplicates(t_exec **array, int *count)
{
	int		i;
	int		k;
	char	*a;
	char	*b;
		
	i = 0;
	while (i < *count - 1)
	{
		a = basename(array[i]->cmd);
		b = basename(array[i + 1]->cmd);
		if (ft_strcmp(a, b) == 0)
		{
			k = i + 1;
			while (k < *count)
			{
				array[k] = array[k + 1];
				k++;
			}
			array[k] = NULL;
			(*count)--;
			continue;
		}
		i++;
	}
}

void	tab_print_columns(t_exec **array, int count)
{
	int				max_len;
	int				column_width;
	struct winsize	ws;
	int				num_columns;
	int				num_rows;
	int				i;
	int				j;
	int				k;
	int				flag;

	
	ft_printf(1, "\n");
	tab_bubble_sort(array, count);
	tab_remove_duplicates(array, &count);
	max_len = tab_max_length(array);
	if (ioctl(0, TIOCGWINSZ, &ws) == -1)
		return ;
	column_width = max_len + 2;
	num_columns	= ws.ws_col / column_width;
	num_rows = 0;
	while (num_rows * num_columns < count)
		num_rows++;
	i = 0;
	while (i < num_rows)
	{
		j = 0;
		while (j < num_columns)
		{
			k = i + (j * num_rows);
			if (k < count)
			{
				flag = 1;
				ft_printf(1, "%s", basename(array[k]->cmd));
				k = ft_strlen(basename(array[k]->cmd));
				if (j < num_columns - 1)
				{
					while (k++ < column_width)
						ft_printf(1, " ");
				}
				else
				{
					flag = 0;
					ft_printf(1, "\n");
				}
			}
			else
			{
				if (flag == 1)
				{
					flag = 0;
					ft_printf(1, "\n");
				}
			}
			j++;
		}
		i++;
	}
	if (flag)
		ft_printf(1, "\n");

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
			tab_print_columns(matches, i);
			reprint_input(sh);
/*			ft_printf(1, "\n");
			i = 0;
			while (matches[i] != 0)
			{
				ft_printf(1, "%s\n", basename(matches[i]->cmd));
				i++;
			}
*/		}
		free(matches);
		reprint_input(sh);
	}
	return ;
}
