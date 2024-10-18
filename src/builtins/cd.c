/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ojacobs <ojacobs@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 19:44:43 by ojacobs           #+#    #+#             */
/*   Updated: 2024/10/18 16:07:39 by ojacobs          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_cd(const char *path)
{
	char	cwd[PATH_MAX];

	if (chdir(path) == 0)
	{
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			printf("Directory changed to: %s\n", cwd);
		}
		else
		{
			perror("getcwd() error");
		}
	}
	else
	{
		// Print an error message if changing the directory fails
		perror("chdir() error");
	}
}

int main(int argc, char *argv[]) {
	// Check if a path is provided as an argument
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <path>\n", argv[0]);
		return 1;
	}

	// Call the custom cd function with the provided path
	ft_cd(argv[1]);

	return 0;
}

// Different implementation of cd

static void	ft_print_error(char **args)
{
	ft_putstr_fd("cd: ", STDERR);
	if (args[2])
		ft_putchar_fd("String not in pwd: ", STDERR);
	else
	{
		ft_putstr_fd(strerror(errno), STDERR);
		ft_putchar_fd(": ", STDERR);
	}
	ft_putendl_fd(args[1], STDERR);
}

static  char  *ft_get_env_path(t_env_variable *env, const *var, size_t len)
{
	char  *old_pwd;
	int  i;
	int j;
	int s_alloc;

	while (env && env-env->next)
	{
		if (ft_strncmp(env->variable, var, len) == 0)
		{
			s_alloc = ft_strlen(env->variable) - len;
			if (!(old_pwd = (char *)malloc(sizeof(char) * (s_alloc + 1))))
				return (NULL);
			i = 0;
			j = 0;
			while (env->variable[i++])
			{
				if (i > (int)len)
					old_pwd[j++] = env->variable[i];
			}
			old_pwd[j] = '\0';
			return (old_pwd);
		}
		env = env->next;
	}
	return (NULL);
}

char		*ft_get_env_name(char *dest, const char *src)
{
	int		i;

	i = 0;
	while (src[i] && src[i] != '=' && ft_strlen(src) < BUFF_SIZE)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}


int			ft_is_in_env(t_env_variable *env, char *args)
{
	char	var_name[BUFF_SIZE];
	char	env_name[BUFF_SIZE];

	ft_get_env_name(var_name, args);
	while (env && env->next)
	{
		ft_get_env_name(env_name, env->variable);
		if (ft_strcmp(var_name, env_name) == 0)
		{
			ft_memdel(env->variable);
			env->variable = ft_strdup(args);
			return (1);
		}
		env = env->next;
	}
	return (SUCCESS);
}

static int ft_update_old_pwd(t_env_variable *env)
{
	char	cwd[PATH_MAX];
	char	*old_pwd;

	if (getcwd(cwd, PATH_MAX) == NULL )
		return (ERROR);
	if (!(old_pwd = ft_strjoin("OLDPWD=", cwd)))
		return (ERROR);
	if (ft_is_in_env(env, old_pwd) == SUCCESS)
		ft_env_update(old_pwd, env);
	ft_memdel(old_pwd);
	return (SUCCESS);
}

static int	ft_go_to_path(int is_old, t_env_variable *env)
{
	int 	return_cd;
	char	*path;

	path = NULL;
	if (is_old == 0)
	{
		ft_update_old_pwd(env);
		path = ft_get_env_path(env, "HOME", 4);
		if (!path)
			ft_putendl_fd("cd: HOME not set", STDERR);
		if (!path)
			return (ERROR);
	}
	else if (is_old == 1)
	{
		path = ft_get_env_path(env, "OLDPWD", 6);
		if (!path)
		{
			ft_putendl_fd("cd: OLDPWD not set", STDERR);
			return (ERROR);
		}
		ft_update_old_pwd(env);
	}
	return_cd = chdir(path);
	ft_memdel(path);
	return (return_cd);
}

int	ft_cd(char **args, t_env_variable *env)
{
	int		return_cd;

	if (!args[1])
		return (ft_go_to_path(0, env));
	if (ft_strcmp(args[1], "-") == 0)
		return_cd = ft_go_to_path(1, env);
	else
	{
		ft_update_old_pwd(env);
		return_cd = chdir(args[1]);
		if (return_cd < 0)
			return_cd *= -1;
		if (return_cd != 0)
			ft_print_error(args);
	}
	return (return_cd);
}