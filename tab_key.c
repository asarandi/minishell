/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tab_key.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/06 05:25:17 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/06 05:35:29 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	key_tab_function(t_shell *sh)
{
	//when one match only, autocomplete (replace) current word with match and append space,
	//when multiple matches show a list of matches in columns, like ls,
	//when over a hundred matches, prompt user for display, (calculate number of output lines)
	//
	//if first word of command then suggest executables from $PATH folders
	//if not first word, suggest files from current folder, or expand partial input
	//
	//autocomplete
	//
//	ft_printf(STDOUT_FILENO, "tab\n");
	sh->buf_i += 0;
}
