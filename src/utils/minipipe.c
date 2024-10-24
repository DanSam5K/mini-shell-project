/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minipipe.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ojacobs <ojacobs@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 17:18:02 by ojacobs           #+#    #+#             */
/*   Updated: 2024/10/24 17:34:45 by ojacobs          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	create_pipe(int pipefd[2])
{
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return -1;
    }
    return 0;
}

// Helper function to handle the child process
int	handle_child_process(int pipefd[2], t_shell_state *shell_state)
{
    close(pipefd[1]); // Close the write end of the pipe

    if (dup2(pipefd[0], STDIN_FILENO) == -1) // Redirect pipe input to STDIN
    {
        perror("dup2");
        return -1;
    }
    close(pipefd[0]);

    shell_state->pipe_input_fd = STDIN_FILENO; // Set input_fd in shell state
    shell_state->proc_id = -1;                 // Mark as child process
    shell_state->is_parent_proc = 0;           // It's not the parent process

    return 2; // Child process return code
}

int	handle_parent_process(int pipefd[2], pid_t pid, t_shell_state *shell_state)
{
    close(pipefd[0]); // Close the read end of the pipe

    if (dup2(pipefd[1], STDOUT_FILENO) == -1) // Redirect pipe output to STDOUT
    {
        perror("dup2");
        return -1;
    }
    close(pipefd[1]);

    shell_state->pipe_output_fd = STDOUT_FILENO; // Set output_fd in shell state
    shell_state->proc_id = pid;                  // Set the child process ID
    shell_state->last_exit_stat = 0;           // Reset last exit status

    return 1; // Parent process return code
}