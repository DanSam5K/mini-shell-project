/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ojacobs <ojacobs@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 20:00:14 by dsamuel           #+#    #+#             */
/*   Updated: 2024/10/12 18:52:33 by ojacobs          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef struct token {
	char **cmd:
	char *args
	int pipe 
	char *infile;
	char outfile;
	int herdoc;
}

void	*ft_memdel(void *ptr)
{
	if (ptr)
	{
		free(ptr);
		ptr = NULL;
	}
	return (NULL);
}

void	ft_sig_integer(int signal)
{
	(void)signal;
	if (global_sig.child_proc_id == 0)
	{
		ft_putstr_fd("\b\b", STDERR);
		ft_putstr_fd("\n", STDERR);
		ft_putstr_fd("\033[0;36m\033[1m minishell ▸ \033[0m", STDERR);
		global_sig.last_exit_stat = 1;
	}
	else
	{
		ft_putstr_fd("\n", STDERR);
		global_sig.last_exit_stat = 130;
	}
	global_sig.sigint_received = 1;
}

void	ft_sig_exit(int signal)
{
	char	*nbr;

	nbr = ft_itoa(signal);
	if (global_sig.child_proc_id != 0)
	{
		ft_putstr_fd("\b\b \b\b", STDERR);
		ft_putendl_fd(nbr, STDERR);
		global_sig.last_exit_stat = 131;
		global_sig.sigquit_received = 1;
	}
	else
		ft_putstr_fd("\b\b  \b\b", STDERR);
	ft_memdel(nbr);
}

void	ft_sig_init(void)
{
	global_sig.sigint_received = 0;
	global_sig.sigquit_received = 0;
	global_sig.last_exit_stat = 0;
	global_sig.child_proc_id = 0;
}
