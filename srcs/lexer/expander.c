/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julolle- <julolle-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 12:09:34 by julolle-          #+#    #+#             */
/*   Updated: 2023/10/27 18:26:54 by julolle-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_error(char *str, int *i, t_exec *exec)
{
	char	*str_final;
	char	*str_exp;

	str_exp = ft_itoa(exec->exit[1]);
	if (!str_exp)
		return (NULL);
	str_final = create_new_str(str, str_exp, i, 1);
	free(str_exp);
	free(str);
	return (str_final);
}

char	*find_dollar_sign(char *str, t_exec *exec, int *flag_exp)
{
	int		i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			if (str[i + 1] && str[i + 1] == '?')
				str = expand_error(str, &i, exec);
			else if (str[i + 1] && ('0' <= str[i + 1] && str[i + 1] <= '9'))
				str = expand_num_null(str, &i);
			else
				str = check_expand(str, &i, exec, flag_exp);
			if (!str)
				return (NULL);
		}
		i++;
	}
	return (str);
}

int	skip_hdoc(t_tok **lst_tok)
{
	t_tok	*tmp;

	if (!(*lst_tok))
		return (0);
	tmp = ft_lstlast_tok(*lst_tok);
	while (tmp && tmp->type <= 2)
		tmp = tmp->prev;
	if (tmp && tmp->type == 7)
		return (1);
	else if (tmp && tmp->type == 3 && tmp->prev && tmp->prev->type == 7)
		return (1);
	return (0);
}

char	*expander(t_tok **lst_tok, char *str, t_exec *exec, int type)
{
	char	*str_exp;
	int		flag_exp;

	flag_exp = 0;
	if (skip_hdoc(lst_tok))
	{
		new_tok(lst_tok, str, type, &exec->exit[0]);
		return (NULL);
	}
	str_exp = find_dollar_sign(str, exec, &flag_exp);
	if (!str_exp)
	{
		err_msg_parser(MALLOC_MESS, 12, 0, &exec->exit[0]);
		return (NULL);
	}
	if (ft_strlen(str_exp) == 0)
		return (str_exp);
	else if (flag_exp == 1 && type == 2)
		split_tok(lst_tok, str_exp, &exec->exit[0]);
	else
		new_tok(lst_tok, ft_strdup(str_exp), type, &exec->exit[0]);
	return (str_exp);
}

int	remove_dol_end(t_tok **lst_tok, int *exit)
{
	t_tok	*tmp;
	char	*sub_str;

	if (!(*lst_tok))
		return (0);
	tmp = ft_lstlast_tok(*lst_tok);
	if (tmp && tmp->type == 2 && tmp->str[ft_strlen(tmp->str) - 1] == '$')
	{
		sub_str = ft_substr(tmp->str, 0, ft_strlen(tmp->str) - 1);
		if (!sub_str)
			return (err_msg_parser(MALLOC_MESS, 12, 0, exit));
		free(tmp->str);
		tmp->str = sub_str;
	}
	return (0);
}
