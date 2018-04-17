/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   norme.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/08 16:39:20 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/16 16:52:40 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_av	*init_av_buffers(t_shell *sh)
{
	t_av	*av;

	sh->child_argv = ft_memalloc(sizeof(char *));
	sh->child_argv[0] = NULL;
	if ((av = ft_memalloc(sizeof(t_av))) == NULL)
		return (NULL);
	av->in = sh->buffer;
	if ((av->out = ft_memalloc(PAGESIZE * 2)) == NULL)
	{
		cleanup_av_buffers(av);
		return (NULL);
	}
	if ((av->key = ft_memalloc(PAGESIZE)) == NULL)
	{
		cleanup_av_buffers(av);
		return (NULL);
	}
	av->val = NULL;
	return (av);
}
