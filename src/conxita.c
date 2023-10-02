/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conxita.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: blvilarn <blvilarn@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/02 12:48:44 by blvilarn          #+#    #+#             */
/*   Updated: 2023/10/02 14:16:57 by blvilarn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../conxita.h"

static void	print_conxita(void)
{
	int		fd;
	char	*line;

	fd = open("src/conxita.txt", O_RDONLY);
	if (fd < 0)
		return ;
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free (line);
		line = get_next_line(fd);
	}
	close(fd);
}

int	main(void)
{
	char	*prompt;

	print_conxita();
	while ("Conxita")
	{
		printf("%s", "conxita$ ");
		prompt = readline("conxita$ ");
		if (prompt)
			printf("%s\n", prompt);
		free (prompt);
	}
	return (0);
}
