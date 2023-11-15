#include "../../conxita.h"

char	*get_path(t_env *env, char *str)
{
	char	*path;
	char	*test;
	int		i;
	int		total;

	path = NULL;
	i = 0;
	if (access(str, X_OK) == 0)
		return (str);
	while (env && ft_strncmp(env->key, "PATH", 4))
		env = env->next;
	total = path_count(env->value, ':');
	while (access(path, X_OK) != 0 && i < total)
	{
		free(path);
		test = minisplit(env->value, i++);
		path = ft_strjoin(test, str);
		free(test);
	}
	if (access(path, X_OK) == 0)
		return (path);
	free(path);
	return (NULL);
}
/*
t_cmd	*init_cmd_test(void)
{
	t_cmd	*cmd;

	cmd = (t_cmd *)malloc(sizeof(t_cmd) * 3);
	if (!cmd)
		return (NULL);

	cmd[0].cmd = (char **)malloc(sizeof(char *) * 4);
	cmd[0].cmd[0] = ft_strdup("/bin/echo");
	cmd[0].cmd[1] = ft_strdup("hola");
	cmd[0].cmd[2] = NULL;//ft_strdup("file1");
	cmd[0].cmd[3] = NULL;
	cmd[0].heredoc = NULL;
	cmd[0].infile = ft_strdup("file1");
	cmd[0].infd = open(cmd[0].infile, O_RDONLY);
	if (cmd[0].infd < 0)
		exit(printf("open cmd[0] potxo\n"));
	cmd[0].outfile = NULL;
	cmd[0].outfd = -1;
	cmd[0].output = opipe;
	cmd[0].input = infile;

	cmd[1].cmd = (char **)malloc(sizeof(char *) * 3);
	cmd[1].cmd[0] = ft_strdup("/usr/bin/wc");
	cmd[1].cmd[1] = NULL;
	cmd[1].heredoc = NULL;
	cmd[1].infile = NULL;
	cmd[1].infd = -1;
	cmd[1].outfile = NULL;
	cmd[1].outfd = -1;
	cmd[1].output = opipe;
	cmd[1].input = ipipe;

	cmd[2].cmd = (char **)malloc(sizeof(char *) * 3);
	cmd[2].cmd[0] = ft_strdup("/bin/ls");
	cmd[2].cmd[1] = NULL;
	cmd[2].cmd[2] = NULL;
	cmd[2].heredoc = NULL;
	cmd[2].infile = NULL;
	cmd[2].infd = -1;
	cmd[2].outfile = ft_strdup("outfile");
	cmd[2].outfd = -1;
	cmd[2].output = f_trunc;
	cmd[2].input = ipipe;
	return (cmd);
}*/

int	redirections(t_cmd *cmd, t_redir *redir)
{
	int	err;

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

int	get_out_fd(t_cmd *cmd)
{
	int	fd;

	if (cmd->output == f_trunc)
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	else if (cmd->output == f_append)
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0664);
	if ((cmd->output == f_trunc || cmd->output == f_append) && fd == -1)
	{
		printf("Bad file descriptor\n");
		exit(127);
	}
	return (fd);
}

pid_t	exec_cmd(t_cmd *cmd, t_redir *redir)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
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

int	ft_waitpid(int pid)
{
	int	status;
	int	wait;

	wait = waitpid(-1, &status, 0);
	if (wait == -1)
		return (print_errors("waitpid"));
	if (wait == pid)
	{
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			return (WTERMSIG(status));
		else if (WIFSTOPPED(status))
			return (WSTOPSIG(status));
	}
	return (0);
}

t_redir	*init_redir()
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->saved_std[0] = dup(0);
	redir->saved_std[1] = dup(1);
	if (redir->saved_std[0] == -1 || redir->saved_std[1] == -1)
		return (NULL);
	redir->fdr_aux = -1;
	return (redir);
}

int	lets_execute(t_cmd *cmd, t_redir *redir, int len)
{
	int		i;
	pid_t	pid;

	i = 0;
	if (!cmd || !redir)
		return (print_errors(NULL));
	while (i < len)
	{
		if (pipe(redir->fd_pipe) == -1)
			return (-1);
		cmd[i].outfd = get_out_fd(&cmd[i]);
		pid = exec_cmd(&cmd[i], redir);
		if ((cmd[i].outfile && close(cmd[i].outfd) == -1)
			|| close(redir->fd_pipe[1]) == -1
			|| (redir->fdr_aux > 0 && close(redir->fdr_aux) == -1))
		{
			printf("close has failed\n");
			return (print_errors(NULL));
		}
		redir->fdr_aux = redir->fd_pipe[0];
		i++;
	}
	if (close(redir->saved_std[0]) == -1 || close(redir->saved_std[1]) == -1
		|| close(redir->fd_pipe[0]) == -1)
		return (-1);
	return (pid);
}
/*
int	main(void)
{
	t_cmd	*cmd;

	cmd = init_cmd_test();
	//int pid = lets_execute(cmd, 3);
	//ft_waitpid(pid);
	if (fork() == 0)
	{
		if (execve(cmd[0].cmd[0], cmd[0].cmd, NULL))
		{
			exit(printf("FUCK\n"));
		}
	}
	exit(1);
}*/
/*
int	main(int argc, char **argv, char **env)
{
	(void)argc;
	(void)argv;
	t_env *tenv = env_to_lst(env);
	char	*path = get_path(tenv, "cat");
	printf("pipes.c: path %s\n", path);
	exit(1);
}*/
