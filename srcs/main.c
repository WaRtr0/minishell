/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 04:37:28 by gcros             #+#    #+#             */
/*   Updated: 2024/03/21 05:22:41 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include <stdio.h>

int	main(int ac, char **av, char **env)
{
	(void) ac;
	(void) av;
	(void) env;
	char **s;

	s = env;
	while (*s != NULL)
	{
		printf("%s\n", *s);
		s++;
	}
	return (0);
}
