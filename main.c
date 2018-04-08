/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/30 19:51:05 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/08 05:31:59 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clean_up(t_shell *sh)
{
	termios_restore_settings(sh);
	destroy_list_of_executables(sh);
	if (sh->envp != NULL)
		destroy_char_array(sh->envp);
	if (sh->history != NULL)
		destroy_char_array(sh->history);
	if (sh->child_argv != NULL)
		destroy_char_array(sh->child_argv);
	if (sh->buffer != NULL)
		free(sh->buffer);
	if (sh != NULL)
		free(sh);
}

void	fatal_error(t_shell *sh)
{
	clean_up(sh);
	exit(EXIT_FAILURE);
}

void	fatal_error_message(t_shell *sh, char *msg)
{
	ft_printf(STDERR_FILENO, "%s: error: %s\n", SHELL_NAME, msg);
	fatal_error(sh);
}

void	sigint_handler(int signo)
{
	ft_printf(STDOUT_FILENO, "\n");
	if (signo == SIGINT)
	{
		if (g_sh->state == STATE_READ)
		{
			ft_bzero(g_sh->buffer, g_sh->bufsize);
			g_sh->buf_i = 0;
			g_sh->input_size = 0;
			display_shell_prompt();
		}
		else if (g_sh->state == STATE_EXEC)
		{
		}
	}
}

t_shell	*init_shell(int argc, char **argv, char **envp)
{
	t_shell	*sh;

	(void)signal(SIGINT, sigint_handler);
	if ((sh = ft_memalloc(sizeof(t_shell))) == NULL)
		fatal_error_message(sh, E_NOMEM);
	sh->argc = argc;
	sh->argv = argv;
	sh->envp = create_char_array_copy(envp, 0);
	terminal_init(sh);
	g_sh = sh;
	build_list_of_executables(sh);

/////debug
	t_exec	*ptr;
	ptr = sh->exec;
	while (ptr != NULL)
	{
		ft_printf(STDOUT_FILENO, "%s\n", ptr->cmd);
		ptr = ptr->next;
	}
///
//
	return (sh);
}

int		main(int argc, char **argv, char **envp)
{
	t_shell		*sh;
	int			r;
	static void	(*builtin_functions[]) (t_shell *) = {&builtin_echo,
		&builtin_cd, &builtin_setenv, &builtin_unsetenv, &builtin_env,
		&builtin_exit, &builtin_help};

	sh = init_shell(argc, argv, envp);
	while (1)
	{
		sh->state = STATE_READ;
		raw_read(sh);
		if (sh->buffer == NULL)
			break ;
		sh->state = STATE_EXEC;
		if ((build_child_argv_list(sh, 0, 0, 1) == 1)
				&& (sh->child_argv[0] != NULL))
		{
			if ((r = builtin_cmd_index(sh->child_argv[0])) != -1)
				(void)builtin_functions[r](sh);
			else
				(void)execute_external_cmd(sh);
		}
		clear_input_buffers(sh);
	}
	clean_up(sh);
	return (0);
}
