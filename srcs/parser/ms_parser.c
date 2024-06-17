/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_parser.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmorot <mmorot@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 00:18:57 by mmorot            #+#    #+#             */
/*   Updated: 2024/06/13 23:49:20 by mmorot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"
#include "lexer.h"
#include "conf.h"
#include "str.h"
#include "utils.h"
#include "exec.h"
#include "put.h"

static void	syntax_no_quote(t_parser_str *str, t_shell *shell,
	t_prompt_s *status, char *line)
{
	if (str->type == E_WILDCARD && status->heredoc)
		str->type = E_WORD;
	ms_syntax_rule(str->type,
		ft_select_str(&line[str->index], str->len), shell, status);
	ms_update_status_text(status, str->type);
	ms_update_status_heredoc(status, str->type);
	ms_update_status_operator(status, str->type, str);
	if (str->type == E_PARENTHESIS)
		ms_lexer_parenthesis(shell, status, str);
}

static void	run_parsing(char *line, t_prompt_s *status,
	t_shell *shell)
{
	t_parser_str	str;

	str.index = 0;
	str.str = line;
	while (line[str.index])
	{
		str.type = ms_get_type(&line[str.index]);
		if (str.type == E_EOF)
			break ;
		str.len = ms_get_indent(str.type, &line[str.index]);
		if ((str.type == E_METACHAR
				&& ms_get_metachar(&line[str.index]) == E_AND)
			|| (str.type == E_HEREDOC && (status->squote || status->dquote)))
			str.type = E_WORD;
		if (str.type == E_NAME && status->heredoc)
			str.type = E_WORD;
		if (!status->squote && !status->dquote)
			syntax_no_quote(&str, shell, status, line);
		ms_update_status_quote(status, &str.type);
		if (shell->prompt_listen == 0 || shell->error > 0)
			break ;
		ms_lexer(shell, status, str.type, &str);
		str.index += str.len;
		str.len = 0;
	}
}

static void	recursive_parser(t_shell *shell, t_prompt_s *status)
{
	char	*newline;
	int		fds[2];

	save_io((int [2]){0, 1}, fds);
	ms_sig_set(sig_heredoc);
	while (status->squote
		|| status->dquote || status->parenthesis || status->newline)
	{
		newline = next_line("> ");
		if (!newline)
		{
			shell->prompt_listen = 0;
			if (g_signal_value != 130)
				ft_putendl_fd(MS_NAME ERR_UNEXPECTED, 2);
			if (g_signal_value != 130)
				ms_set_status(2 << 8);
			break ;
		}
		ms_parser(newline, status, shell);
		free(newline);
		if (shell->prompt_listen == 0)
			break ;
	}
	restore_io(fds);
	ms_sig_set(sig_exec);
}

int	ms_parser(char *line, t_prompt_s *status, t_shell *shell)
{
	if (ft_strlen(line) <= 0)
		return (0);
	ms_join_parsers(shell, status, line);
	shell->prompt_listen = 1;
	status->no_print = 0;
	run_parsing(line, status, shell);
	if (status->chevron)
		ms_syntax_error(E_SYNTAX_UPD_NLN, NULL, shell);
	if (shell->prompt_listen == 0 || shell->error > 0)
	{
		if (ms_get_parent(shell, 1) != NULL
			&& ms_get_parent(shell, 1)->type == CMD_HEREDOC)
		{
			shell->cursor = ft_arr_pop(shell->cursor_array);
			ft_arr_free(&ms_get_cursor(shell)->content.array, NULL);
		}
		return (0);
	}
	recursive_parser(shell, status);
	ms_lexer_heredoc_handle(shell, status);
	return (1);
}