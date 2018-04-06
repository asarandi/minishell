/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   words.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/06 05:45:21 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/06 05:46:01 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// are these even used?

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
