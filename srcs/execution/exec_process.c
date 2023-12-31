/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julolle- <julolle-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 21:20:02 by rjobert           #+#    #+#             */
/*   Updated: 2023/10/26 12:18:06 by julolle-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_machine(t_proc *pcs_chain, t_exec *exec)
{
	if (!pcs_chain)
		return (0);
	if (exec->total_cmd == 1 && is_builtin(pcs_chain))
	{
		exec->in_parent = 1;
		exec->exit[0] = check_io_fd(pcs_chain, exec);
		if (exec->exit[0])
			return (exec->exit[0]);
		io_redirect(pcs_chain, exec);
		exec->exit[0] = exec_builtin(pcs_chain, exec);
		back_up_stdio(exec, 1);
		return (exec->exit[0]);
	}
	exec->in_parent = 0;
	pipefd_calibrate(exec);
	launch_process(exec, &pcs_chain);
	close_all_pipes(exec);
	wait_processes(exec);
	free_exec(&exec);
	return (exec->exit[0]);
}

int	is_builtin(t_proc *pcs_chain)
{
	char	*cmd;

	if (!pcs_chain || !pcs_chain->arg || !pcs_chain->arg[0])
		return (0);
	cmd = pcs_chain->arg[0];
	if (!ft_strncmp(cmd, "cd", 3))
		return (1);
	else if (!ft_strncmp(cmd, "pwd", 4))
		return (1);
	else if (!ft_strncmp(cmd, "echo", 5))
		return (1);
	else if (!ft_strncmp(cmd, "env", 5))
		return (1);
	else if (!ft_strncmp(cmd, "export", 7))
		return (1);
	else if (!ft_strncmp(cmd, "unset", 6))
		return (1);
	else if (!ft_strncmp(cmd, "exit", 5))
		return (1);
	return (0);
}

int	exec_builtin(t_proc *pcs_chain, t_exec *exec)
{
	char	*cmd;

	if (!pcs_chain)
		return (1);
	cmd = pcs_chain->arg[0];
	if (!ft_strncmp(cmd, "cd", 3))
		return (ft_cd(exec, pcs_chain->arg));
	else if (!ft_strncmp(cmd, "pwd", 4))
		return (ft_pwd(exec, pcs_chain->arg));
	else if (!ft_strncmp(cmd, "echo", 5))
		return (ft_echo(exec, pcs_chain->arg));
	else if (!ft_strncmp(cmd, "env", 5))
		return (ft_env(exec, pcs_chain->arg));
	else if (!ft_strncmp(cmd, "export", 7))
		return (ft_export(exec, pcs_chain->arg));
	else if (!ft_strncmp(cmd, "unset", 6))
		return (ft_unset(exec, pcs_chain->arg));
	else if (!ft_strncmp(cmd, "exit", 5))
		return (ft_exit(exec, pcs_chain->arg));
	ft_printf(STDERR_FILENO, "minishell: %s: not a valid option\n", \
	pcs_chain->arg[0]);
	return (EXIT_FAILURE);
}

/*
special error function for builtin execution in parent process
to not exit
*/
int	error_builtin(char *msg, int nb, char *bltn)
{
	if (bltn)
		ft_printf(STDERR_FILENO, "minishell: %s: %s\n", msg, bltn);
	else
		ft_printf(STDERR_FILENO, "minishell: %s\n", msg);
	return (nb);
}

/* 
Objective : apply execve with many error handling
1 - first we check if the the command is a directory using opendir()
2 - if not, and if it is not an emty command (!= "") 
we check if the path is findable and exist - we verify that (again) 
with access F_OK) we then test several cases:
2.a - if the file is not exectutable ( a script.sh where -x- right are
not for user or group -> we return error 126 and exit message)
2.b - else we can apply execve and return the error 1 if issue
3 - if the argument / command start with "/" it is a path to a exe
 or .sh that was provdied -> if it exist with access F_OK we then 
  try to execute it
4 - If no execution we have 2 message for the 127 "not found" exit:
4.a- if the command start with "/" and could not be executed:
 No such file with with exit 127
4.b - otherwise it was a command and not a file -> command not found
*/
void	exec_bash(t_proc **exec_trgt, t_exec **exec)
{
	DIR	*d;

	d = opendir((*exec_trgt)->arg[0]);
	if (d && ft_strchr((*exec_trgt)->arg[0], '/'))
	{
		closedir(d);
		error_msg("is a directory", 126, *exec, *exec_trgt);
	}
	if (access((*exec)->path, F_OK) == 0 && (*exec_trgt)->arg[0][0] != '\0')
	{
		if (access((*exec)->path, X_OK) != 0)
			error_msg(NOPERM_MESS, NOPERM, *exec, *exec_trgt);
		if (execve((*exec)->path, (*exec_trgt)->arg, (*exec)->env) == -1)
			exit(EXIT_FAILURE);
	}
	if (ft_strchr((*exec_trgt)->arg[0], '/'))
		error_msg("No such file or directory", CMNOFOUND, *exec, *exec_trgt);
	error_msg(CMNF_MESS, CMNOFOUND, *exec, *exec_trgt);
}
