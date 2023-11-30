#include "../../conxita.h"

/*Returns an isolated path from the environment variable*/
char	*get_path(t_env *env, char *str)
{
	char	*path;
	char	*test;
	int		i;
	int		total;

	path = NULL;
	i = 0;
	if (is_builtin(str))
		return (NULL);
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

/*Initializes char **cmd (NULL-terminated) from struct t_cmd.
 It tries to add a path to the first element. If no valid path is found,
 it will put the received value (token[i].val)*/
int	init_cmd_cmd(t_oken *token, t_cmd *cmd, int i, t_env *env)
{
	int	j;
	int	len;

	j = -1;
	if (!token)
		return (-1);
	len = arg_count(token, i);
	cmd->cmd = (char **)malloc(sizeof(char *) * (len + 1));
	if (!cmd->cmd)
		exit((unsigned char)print_errors(NULL));
	while (++j < len && token[i].val && token[i].type == arg)
	{
		if (j == 0)
			cmd->cmd[j] = get_path(env, token[i].val);
		if (j != 0 || !cmd->cmd[j])
			cmd->cmd[j] = ft_strdup(token[i].val);
		if (!cmd->cmd[j])
			exit((unsigned char)print_errors(NULL));
		i++;
	}
	cmd->cmd[j] = NULL;
	return (i);
}

/*initializes t_cmd (except for the char **cmd variable)*/
int	init_cmd_red(t_conxita *all, t_cmd *cmd, int *i)
{
	int			ret;

	ret = 0;
	if (all->token[*i].val[0] == '<')
		ret = init_chev_input(all->token, cmd, i);
	else if (all->token[*i].val[0] == '>')
		ret = init_chev_output(all->token, cmd, i);
	all->exit = ret;
	if (all->exit == -1)
		all->exit = 1;
	(*i)++;
	return (ret);
}
