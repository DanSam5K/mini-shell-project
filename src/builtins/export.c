/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ojacobs <ojacobs@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/18 16:04:46 by ojacobs           #+#    #+#             */
/*   Updated: 2024/10/18 16:05:24 by ojacobs          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char		*ft_get_env_name(char *dest, const char *src);
int			ft_is_in_env(t_env_variable *env, char *args);
static int ft_print_error_export(int return_err, const char *arg)
{
	int i;
	if (return_err == -1)
		ft_putstr_fd("export: not a valid identifier: ", STDERR);
	else if (return_err == 0 || return_err == -3)
		ft_putstr_fd("not a valid identifier: ", STDERR);
	i = 0;
	while (arg[i] && (arg[i] != '=' || return_err == -3))
	{
		write(STDERR, &arg[i], 1);
		i++;
	}
	write(STDERR, "\n", 1);
	return (ERROR);
}
int ft_add_to_env(const char *args, t_env_variable *env)
{
	t_env_variable	*new_env;
	t_env_variable	*tmp;
	if (env && env->variable == NULL)
	{
		env->variable = ft_strdup(args);
		return (SUCCESS);
	}
	if (!(new_env = (t_env_variable *)malloc(sizeof(t_env_variable))))
		return (-1);
	new_env->variable = ft_strdup(args);
	while (env && env->next && env->next->next)
		env = env->next;
	tmp = env->next;
	env->next = new_env;
	new_env->next = tmp;
	return (SUCCESS);
}
//this was another way I thought I could achieve ft_add_to_env not totally discarded which ever way you choose to achieve you comment this code and leave it in case we start debugiing i can remember some concept I why i did this
// int ft_add_to_env(char *args, t_env_variable *env)
// {
// 	t_env_variable	*new_env;
// 	t_env_variable	*tmp;
// 	new_env = (t_env_variable *)malloc(sizeof(t_env_variable));
// 	if (!new_env)
// 		return (ERROR);
// 	new_env->variable = ft_strdup(args);
// 	new_env->next = NULL;
// 	if (!env)
// 	{
// 		env = new_env;
// 		return (SUCCESS);
// 	}
// 	tmp = env;
// 	while (tmp->next)
// 		tmp = tmp->next;
// 	tmp->next = new_env;
// 	return (SUCCESS);
// }
int	ft_export(char **args, t_env_variable *env, t_env_variable *secret)
{
	int	new_env;
	int	error_ret;
	new_env = 0;
	if (!args[1])
	{
		ft_print_sorted_env(secret);
		return (SUCCESS);
	}
	else
	{
		error_ret = ft_is_valid_env(args[1]);
		if (args[1][0] == '=')
			error_ret = -3;
		if (error_ret <= 0)
			return (ft_print_err_export(error_ret, args[1]));
		if (error_ret == 2)
			new_env = 1;
		else
			new_env = ft_is_in_env(env, args[1]);
		if (new_env == 0)
		{
			if (error_ret == 1)
				ft_add_to_env(args[1], env);
			ft_add_to_env(args[1], secret);
		}
	}
	return (SUCCESS);
}
