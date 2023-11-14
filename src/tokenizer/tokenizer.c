#include "../../conxita.h" 

//Prompt tokenizer
t_oken	*glorified_ft_split(char *prompt, char **env)
{
	int		token_num;
	t_oken	*tokens;

	prompt = ft_strtrim(expand_env(env, prompt), " ");
	//printf("%s\n", prompt);
	token_num = get_token_num(prompt);
	tokens = ft_calloc(token_num + 1, sizeof(t_oken));
	if (!tokens)
		return (NULL);
	populate_tokens(prompt, tokens);
	free (prompt);
	int	i = 0;
	while (i < token_num)
	{
		printf("tokenizer-> glorified %i. [%s]\ttype %d\n", i, tokens[i].val, tokens[i].type);
		i++;
	}
	return (tokens);
}
