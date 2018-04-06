/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   special_keys.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/06 05:16:43 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/06 06:34:44 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	check_special_keys(t_shell *sh)
{
	int			i;
	const char	*key;
	int			len;

	i = 0;
	while (i < NUM_SPECIAL_KEYS)
	{
		key = special_keys[i];
		len = ft_strlen(key);
		if (ft_strncmp(key, sh->keycode, len) == 0)
		{
			(void)ft_bzero(sh->keycode, 9);
			return (special_key_functions[i](sh));
		}
		i++;
	}
	return ;
}

void	key_left_arrow_function(t_shell *sh)
{
	if (sh->buf_i > 0)
	{
		sh->buf_i--;
		ft_putstr(sh->cursor_move_left);
	}
	return ;
}

void	key_right_arrow_function(t_shell *sh)
{
	if (sh->buf_i < sh->input_size)
	{
		sh->buf_i++;
		ft_putstr(sh->cursor_move_right);
	}
	return ;
}

void	key_backspace_function(t_shell *sh)
{
	size_t	tempo;

	if (sh->buf_i > 0)
	{
		key_left_arrow_function(sh);
		tempo = sh->buf_i;
		while (tempo < sh->input_size)
		{
			sh->buffer[tempo] = sh->buffer[tempo + 1];
			tempo++;
		}
		sh->buffer[tempo] = 0;
		sh->input_size--;
		reprint_input(sh);
	}
	return ;
}

void	key_delete_function(t_shell *sh)
{
	size_t	tempo;

	if (sh->input_size > sh->buf_i)
	{
		tempo = sh->buf_i;
		while (tempo < sh->input_size)
		{
			sh->buffer[tempo] = sh->buffer[tempo + 1];
			tempo++;
		}
		sh->buffer[tempo] = 0;
		sh->input_size--;
		reprint_input(sh);
	}
	return ;
}

void	key_ctrl_a_function(t_shell *sh)
{
	sh->buf_i = 0;
	reprint_input(sh);
	return ;
}

void	key_ctrl_e_function(t_shell *sh)
{
	sh->buf_i = sh->input_size;
	reprint_input(sh);
	return ;
}

void	key_ctrl_k_function(t_shell *sh)
{
	sh->input_size = sh->buf_i;
	ft_bzero(&sh->buffer[sh->buf_i], sh->bufsize - sh->buf_i);
	ft_putstr(sh->clear_till_eol);
	reprint_input(sh);
	return ;
}

void	key_ctrl_l_function(t_shell *sh)
{
	ft_putstr(sh->clear_all);
	reprint_input(sh);
	return ;
}
