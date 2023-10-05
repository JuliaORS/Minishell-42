/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julolle- <julolle-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/02 11:31:22 by julolle-          #+#    #+#             */
/*   Updated: 2023/10/03 20:07:21 by julolle-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void read_handler (int signal, siginfo_t *data, void *n_data)
{
	(void) data;
	(void) n_data;
	if (signal == SIGINT)
	{
		g_exit_sig = 1;
		printf("\n");
		rl_replace_line("", 1);
		rl_on_new_line();
		rl_redisplay();
	}
}

void heredoc_handler(int signal, siginfo_t *data, void *n_data)
{
	(void) data;
	(void) n_data;
	if (signal == SIGINT)
	{
		g_exit_sig = 1;
		printf("\n");
		rl_replace_line("", 1);
		rl_on_new_line();
		rl_redisplay();
		exit(1);
	}
}

void exec_handler(int signal, siginfo_t *data, void *n_data)
{
	(void) data;
	(void) n_data;
 	if (signal == SIGINT)
	{
		g_exit_sig = 130;
		exit(130);
	}
	else if (signal == SIGQUIT)
	{
		g_exit_sig = 131;
		exit(131);
	}
	
}

void    init_signals(int mode, int *err)
{
	struct sigaction    signal;

	signal.sa_flags = SA_RESTART;
	sigemptyset(&signal.sa_mask);
	if (mode == READ)
		signal.sa_sigaction = read_handler;
	else if (mode == HEREDOC)
		signal.sa_sigaction = heredoc_handler;
	else if (mode == EXEC)
		signal.sa_sigaction = exec_handler;
	*err = g_exit_sig;
	sigaction(SIGINT, &signal, NULL);
	sigaction(SIGQUIT, &signal, NULL);
}