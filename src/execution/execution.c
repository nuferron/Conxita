#include "../../conxita.h"

/*Redirects the file desccriptors according with the input*/
int	redirections(t_cmd *cmd, t_redir *redir)
{
	int	err;
	
	//dprintf(2, "\033[1;36mredirections: cmd %s\tinput %d\toutput %d\033[m\n", cmd->cmd[0], cmd->input, cmd->output);
	err = 0;
	if (cmd->input == infile)
		err = dup2(cmd->infd, 0);
	else if (cmd->input == ipipe)
		err = dup2(redir->fdr_aux, 0);
	else if (cmd->input == stdi)
		err = dup2(redir->saved_std[0], 0);
	if (err == -1)
		return (-1);
	if (cmd->output == opipe)
		err = dup2(redir->fd_pipe[1], 1);
	else if (cmd->output == stdo)
		err = dup2(redir->saved_std[1], 1);
	else if (cmd->output == f_trunc || cmd->output == f_append)
		err = dup2(cmd->outfd, 1);
	if (err == -1)
		return (-1);
	if (close(redir->saved_std[0]) == -1 || close(redir->saved_std[1]) == -1
		|| close(redir->fd_pipe[0]) == -1 || close(redir->fd_pipe[1]) == -1)
		return (-1);
	return (0);
}

/*Well, it executes the command... it also returns de pid of the child*/
pid_t	exec_cmd(t_cmd *cmd, t_redir *redir)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	dprintf(2, "exec_cmd says hi\n");
	if (pid == 0)
	{

		if (exec_heredoc(cmd))
		{
			dprintf(2, "exec cmd pid %d\n", pid);
			return (pid);
		}
		dprintf(2, "exec_cmd says hi\n");
		redirections(cmd, redir);
		if (execve(cmd->cmd[0], cmd->cmd, NULL) == -1)
		{
			if (access(cmd->cmd[0], X_OK) == -1)
				printf("conxita: %s: command not found\n", cmd->cmd[0]);
			else
				print_errors(NULL);
			exit(1);
		}
	}
	return (pid);
}

/*Opens and returns the output's fd. It can be truncated (>) or appended (>>)*/
int	get_out_fd(t_cmd *cmd)
{
	int	fd;

	fd = 0;
	if (cmd->infile)
	{
		cmd->infd = open(cmd->infile, O_RDONLY);
		if (cmd->infd == -1)
			return (print_errors(cmd->infile));
	}
	if (cmd->output == f_trunc)
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	else if (cmd->output == f_append)
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0664);
	if (fd == -1)
	{
		return (print_errors(cmd->outfile));
		//printf("Bad file descriptor\n");
		//exit(127);
	}
	return (fd);
}

/*It basically calls the redirection and the execution functions*/
int	lets_execute(t_cmd *cmd, t_redir *redir, int len)
{
	int		i;
	pid_t	pid;

	i = -1;
	if (!cmd || !redir)
		return (print_errors(NULL));
	//exec_heredoc(cmd);
	while (++i < len)
	{
		//dprintf(2, "lets exec says hi\n");
		if (pipe(redir->fd_pipe) == -1)
			return (-1);
		cmd[i].outfd = get_out_fd(&cmd[i]);
		pid = exec_cmd(&cmd[i], redir);
		//if (cmd[i].heredoc)
		//	continue ;
		if ((cmd[i].outfile && close(cmd[i].outfd) == -1)
			|| close(redir->fd_pipe[1]) == -1
			|| (redir->fdr_aux > 0 && close(redir->fdr_aux) == -1))
			return (print_errors(NULL));
		redir->fdr_aux = redir->fd_pipe[0];
	}
	if (close(redir->saved_std[0]) == -1 || close(redir->saved_std[1]) == -1
		|| close(redir->fd_pipe[0]) == -1)
		return (-1);
	return (pid);
}
