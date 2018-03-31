/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/30 19:50:54 by asarandi          #+#    #+#             */
/*   Updated: 2018/03/31 05:56:42 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include "libft.h"
#include "libftprintf.h"
#include <term.h>


typedef struct	s_shell
{
	int			argc;
	char		**argv;
	char		**envp;
	char		*buffer;
	size_t		bufsize;
}				t_shell;

void	builtin_echo(t_shell *sh);
void	builtin_cd(t_shell *sh);
void	builtin_setenv(t_shell *sh);
void	builtin_unsetenv(t_shell *sh);
void	builtin_env(t_shell *sh);
void	builtin_exit(t_shell *sh);
void	builtin_help(t_shell *sh);

#endif
