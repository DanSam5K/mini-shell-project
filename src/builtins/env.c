/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ojacobs <ojacobs@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 22:07:35 by ojacobs           #+#    #+#             */
/*   Updated: 2024/10/18 22:50:29 by ojacobs          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//This will print all the enviroment variables as a 2D array.

int	ft_env(char **env)
{
	int	i;

	i = 0;
	while (env[i] && (env[i + 1]) != NULL)
	{
		ft_putendl_fd(env[i], STDOUT);
		i++;
	}
	if (env[i])
		ft_putendl_fd(env[i], STDOUT);
	return (SUCCESS);
}


//This is a similar implementation considering our t_env_varaible
int	ft_env(t_env_variable *env)
{
	while (env && env->next != NULL)
	{
		ft_putendl(env->variable);
		env = env->next;
	}
	if (env)
		ft_putendl_fd(env->variable, STDIN_FILENO);
	return (SUCCESS);
}


// int main(int argc, char **av, char **envp)
// {
// 	int res = ft_env(envp);
// }