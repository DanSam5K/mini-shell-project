/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ojacobs <ojacobs@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 22:23:45 by dsamuel           #+#    #+#             */
/*   Updated: 2024/10/24 17:39:53 by ojacobs          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/ioctl.h>
# include <sys/stat.h>
# include <linux/limits.h>
# include <errno.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>

# define EMPTY 0
# define CMD 1
# define ARG 2
# define TRUNC 3
# define APPEND 4
# define INPUT 5
# define PIPE 6
# define END 7

# define STDIN 0
# define STDOUT 1
# define STDERR 2

# define SKIP 1
# define NOSKIP 0

# define BUFF_SIZE 4096
# define EXPANSION -36
# define ERROR 1
# define SUCCESS 0
# define IS_DIRECTORY 126
# define UNKNOWN_CMD 127

/*
** Structure for storing a command token (e.g., a command, argument, pipe, etc.).
The actual string representation of the token (e.g., a command or argument).
Type of token (e.g., command, argument, pipe, etc.).
Pointer to the previous token in the list.
Pointer to the next token in the list.
 */
typedef struct s_cmd_token
{
	char				*content;
	int					type;
	struct s_cmd_token	*prev;
	struct s_cmd_token	*next;
}					t_cmd_token;


/*
** Structure for storing an environment variable.
The full string representation of the env variable (e.g., "PATH=/usr/bin").
Pointer to the next environment variable in the list.
 */
typedef struct s_env_variable
{
	char					*variable;
	struct s_env_variable	*next;
}				t_env_variable;


/*
** Structure for storing the shell state.
Pointer to the list of command tokens (parsed user input).
Pointer to the list of active environment variables.
Pointer to a list of hidden or secret environment variables.
File descriptor for standard input (used for redirection).
File descriptor for standard output (used for redirection).
File descriptor for input redirection.
File descriptor for output redirection.
File descriptor for the input end of a pipe.
File descriptor for the output end of a pipe.
Process ID of the currently executing child process.
Flag indicating if the process is running in the foreground.
Flag indicating if the current process is the parent process.
Stores the exit status of the last executed command.
Stores the return code for the current command.
Flag indicating if the shell should exit.
Flag indicating if command execution should be skipped.
 */
typedef struct s_shell_state
{
	t_cmd_token		*cmd_list;
	t_env_variable	*active_env;
	t_env_variable	*secret_env;
	int				stdin_fd;
	int				stdout_fd;
	int				input_fd;
	int				output_fd;
	int				pipe_input_fd;
	int				pipe_output_fd;
	int				proc_id;
	int				is_foreground;
	int				is_parent_proc;
	int				last_exit_stat;
	int				return_code;
	int				should_exit;
	int				should_skip_exec;
}				t_shell_state;

/*
** Structure for storing signal handling data.
Indicates if a SIGINT (Ctrl+C) signal was received.
Indicates if a SIGQUIT (Ctrl+\) signal was received.
Stores the exit status affected by a signal.
Process ID of the child process affected by a signal.
 */
typedef struct s_sig_handler
{
	int				sigint_received;
	int				sigquit_received;
	int				last_exit_stat;
	pid_t			child_proc_id;
}				t_sig_handler;

/*
** Structure for storing data related to command execution.
Buffer for storing the expanded string (e.g., after expanding `$HOME`).
Index for iterating through the source string during expansion.
Index for writing into the destination buffer during expansion.
 */
typedef struct s_expand_data
{
	char			*expanded_str;
	int				src_index;
	int				dest_index;
}				t_expand_data;

/*
** BUILTINS
*/
int		ft_cd(char **args, t_env_variable *env);
int		ft_echo(char **args);
int		ft_env(t_env_variable *env);
void	ft_mini_exit(t_shell_state *shell_state, char **cmd_args);
int		ft_add_to_env(const char *args, t_env_variable *env);
char	*ft_get_env_name(char *dest, const char *src);
int		ft_is_in_env(t_env_variable *env, char *args);
int		ft_export(char **args, t_env_variable *env, t_env_variable *secret);
int		ft_pwd(void);
int		ft_unset(char **args, t_shell_state *state);

/*
** PARSING
*/

/*
** ENVIRONMENT
*/
size_t	ft_size_env(t_env_variable *lst);
char	*ft_env_to_str(t_env_variable *lst);
int		ft_env_init(t_shell_state *shell_state, char **env_array);
int		ft_secret_env_init(t_shell_state *shell_state, char **env_array);
int		is_env_char(int c);
int		ft_is_valid_env(const char *env);
int		ft_env_value_len(const char *env);
char	*ft_env_value(char *env);
char	*ft_get_env_value(char *arg, t_env_variable *env);
int		ft_str_env_len(char **env);
void	ft_sort_env(char **tab, int env_len);
void	ft_print_sorted_env(t_env_variable *env);
/*
** REDIRECTIONS
*/

void	redir_output(t_shell_state *shell_state, t_cmd_token *token, int type);
void	redir_input(t_shell_state *shell_state, t_cmd_token *token);
int		minipipe(t_shell_state *shell_state);
int		ft_redir_iop(char *ops, t_shell_state *shell_state, t_cmd_token *token);
/*
** SIGNALS HANDLING
*/

void	*ft_memdel(void *ptr); // remember to move to libft
void	ft_sig_integer(int signal);
void	ft_sig_exit(int signal);
void	ft_sig_init(void);

/*
** UTILS
*/
void	ft_free_token(t_cmd_token *start);
void	ft_free_env(t_env_variable *env);
void	ft_free_tab(char **tab);
int		create_pipe(int pipefd[2]);
int		handle_child_process(int pipefd[2], t_shell_state *shell_state);
int		handle_parent_process(int pipefd[2], pid_t pid, t_shell_state *shell_state);

/*
** External functions
*/
extern t_sig_handler global_sig;

#endif