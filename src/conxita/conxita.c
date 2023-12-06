#include "../../conxita.h"

static void	add_shlvl(t_env *env)
{
	char	*new_shlvl;
	t_env	*shlvl;

	shlvl = search_env(env, "SHLVL");
	if (shlvl == NULL)
		return ;
	new_shlvl = ft_itoa(ft_atoi(shlvl->value) + 1);
	free(shlvl->value);
	shlvl->value = new_shlvl;
}

static void	init_all(t_conxita *all, char **env)
{
	all->cmd = NULL;
	all->env = env_to_lst(env);
	all->token = NULL;
	all->redir = NULL;
	all->exit = 0;
	add_shlvl(all->env);
}

static char	*get_prompt(t_conxita *all, char *str)
{
	char	*prompt;

	if (search_env(all->env, "USER")->value != NULL)
		str = ft_strjoin(search_env(all->env, "USER")->value, "@conxita$ ");
	else
		str = ft_strdup("[unknown]@conxita$ ");
	prompt = readline(str);
	ft_dprintf(2, "get_prompt: prompt %s\n", prompt);
	if (!prompt)
	{
		//if (isatty(STDIN_FILENO))
		//	write(2, "exit\n", 6);
		system("leaks minishell");
		exit(all->exit);
	}
	free(str);
	return (prompt);
}

int	main(int argc, char **argv, char **env)
{
	char		*prompt;
	t_conxita	all;

	(void)argv;
	if (argc > 1)
		return (printf("Too many arguments\n"));
	print_conxita();
	setup_signals();
	init_all(&all, env);
	while ("Conxita")
	{
		prompt = ft_strjoin(search_env(all.env, "USER")->value, "@conxita$ ");
		while ("Conxita")
		{
			ft_dprintf(2, "fuck you mainn\n");
			prompt = get_prompt(&all, prompt);
			handle_prompt(prompt, &all);
			free_all(&all);
			free(prompt);
		}
	}
	return (0);
}
