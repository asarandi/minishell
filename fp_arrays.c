/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   f_arrays.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/06 06:27:49 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/06 06:28:44 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

const char *builtin_list[] = {
	"echo",
	"cd",
	"setenv",
	"unsetenv",
	"env",
	"exit",
	"help"
};

void (*builtin_functions[]) (t_shell *) = {
	&builtin_echo,
	&builtin_cd,
	&builtin_setenv,
	&builtin_unsetenv,
	&builtin_env,
	&builtin_exit,
	&builtin_help
};

const char *special_keys[] = {
	KEY_UP_ARROW,
	KEY_DOWN_ARROW,
	KEY_LEFT_ARROW,
	KEY_RIGHT_ARROW,
	KEY_BACKSPACE,
	KEY_DELETE,
	KEY_CTRL_A,
	KEY_CTRL_E,
	KEY_CTRL_K,
	KEY_CTRL_L,
	KEY_TAB
};

void	(*special_key_functions[]) (t_shell *) = {
	&key_up_arrow_function,
	&key_down_arrow_function,
	&key_left_arrow_function,
	&key_right_arrow_function,
	&key_backspace_function,
	&key_delete_function,
	&key_ctrl_a_function,
	&key_ctrl_e_function,
	&key_ctrl_k_function,
	&key_ctrl_l_function,
	&key_tab_function
};
