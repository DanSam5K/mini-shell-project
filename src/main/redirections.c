/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ojacobs <ojacobs@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 15:28:09 by ojacobs           #+#    #+#             */
/*   Updated: 2024/10/24 17:42:13 by ojacobs          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * redir - Handles output redirection to a specified file.
 * 
 * @shell_state: A pointer to the shell state structure (t_shell_state) containing file descriptors.
 * @token: A pointer to the token structure (t_cmd_token) containing the file name for redirection.
 * @type: An integer indicating the type of redirection (`TRUNC` for truncate, append otherwise).
 * 
 * This function manages the redirection of output to a specified file. It closes the 
 * existing output file descriptor and opens a new file for writing. Depending on the 
 * provided `type`, the file is opened in truncate mode (overwrite) or append mode.
 * 
 * If the file fails to open, an error message is displayed, and the return code and 
 * execution flag in `mini` are set accordingly. On success, the file descriptor is 
 * duplicated to `STDOUT` to redirect the output to the newly opened file.
 */

// Pseudo Code for redir
// Close the current output file descriptor (shell_state->output_fd).
// Check the type of redirection:
// If type is TRUNC:
// Open the file specified by token->str with write permissions and truncate it.
// Assign the file descriptor to shell_state->output_fd.
// Else (for append):
// Open the file specified by token->str with write permissions and append mode.
// Assign the file descriptor to shell_state->output_fd.
// Check if opening the file was successful:
// If the file failed to open (file descriptor is -1):
// Print an error message: "minishell: <filename>: No such file or directory".
// Set the return code (shell_state->return_code) to 1.
// Set the no_exec flag in mini to indicate that execution should not proceed.
// Exit the function.
// Duplicate the output file descriptor (shell_state->output_fd) to STDOUT.


void	redir_output(t_shell_state *shell_state, t_cmd_token *token, int type)
{
    int new_fd;

    // Close the current output file descriptor, if it's valid
    if (shell_state->output_fd > 0)
        close(shell_state->output_fd);

    // Check the redirection type (TRUNC or APPEND)
    if (type == TRUNC)
        new_fd = open(token->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else
        new_fd = open(token->content, O_WRONLY | O_CREAT | O_APPEND, 0644);

    // Check if the file was successfully opened
    if (new_fd == -1)
    {
        // Error opening the file, display an error message
        ft_putstr_fd("minishell: ", STDERR);
        ft_putstr_fd(token->content, STDERR);
        ft_putendl_fd(": No such file or directory", STDERR);

        // Set the return code and skip execution
        shell_state->return_code = 1;
        shell_state->should_skip_exec = 1;
        return;
    }

    // Assign the new file descriptor to output_fd
    shell_state->output_fd = new_fd;

    // Redirect STDOUT to the new file descriptor
    if (dup2(shell_state->output_fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        shell_state->return_code = 1;
        shell_state->should_skip_exec = 1;
        close(shell_state->output_fd);
    }
}


/**
 * input - Handles input redirection from a specified file.
 * 
 * @shell_state: A pointer to the shell state structure (t_shell_state) containing file descriptors.
 * @token: A pointer to the token structure (t_cmd_token) containing the file name for redirection.
 * 
 * This function manages the redirection of input from a specified file. It closes the 
 * existing input file descriptor and opens the file in read-only mode. If the file fails 
 * to open, an error message is displayed, and the return code and execution flag in `shell_state` 
 * are set accordingly. On success, the file descriptor is duplicated to `STDIN` to redirect 
 * the input from the newly opened file.
 */

// Pseudo Code for input
// Close the current input file descriptor (shell_sate->input_fd).
// Open the file specified by token->content with read-only permissions.
// Check if opening the file was successful:
// If the file failed to open (file descriptor is -1):
// Print an error message: "minishell: <filename>: No such file or directory".
// Set the return code (shell_state->return_code) to 1.
// Set the no_exec flag in mini to indicate that execution should not proceed.
// Exit the function.
// Duplicate the input file descriptor (shell->input_fd) to STDIN.

void	redir_input(t_shell_state *shell_state, t_cmd_token *token)
{
    int new_fd;

    // Close the current input file descriptor, if it's valid
    if (shell_state->input_fd > 0)
        close(shell_state->input_fd);

    // Open the file in read-only mode
    new_fd = open(token->content, O_RDONLY);

    // Check if the file was successfully opened
    if (new_fd == -1)
    {
        // Error opening the file, display an error message
        ft_putstr_fd("minishell: ", STDERR);
        ft_putstr_fd(token->content, STDERR);
        ft_putendl_fd(": No such file or directory", STDERR);

        // Set the return code and skip execution
        shell_state->return_code = 1;
        shell_state->should_skip_exec = 1;
        return;
    }

    // Assign the new file descriptor to input_fd
    shell_state->input_fd = new_fd;

    // Redirect STDIN to the new file descriptor
    if (dup2(shell_state->input_fd, STDIN_FILENO) == -1)
    {
        perror("dup2");
        shell_state->return_code = 1;
        shell_state->should_skip_exec = 1;
        close(shell_state->input_fd);
    }
}

/**
 * minipipe - Creates a pipe for inter-process communication and forks a new process.
 * 
 * @shell_state: A pointer to the shell state structure (t_shell_state) containing file descriptors and process details.
 * 
 * This function creates a pipe using two file descriptors: one for reading and one for writing. 
 * It forks the current process into a parent and a child. The child process closes the write end 
 * of the pipe, duplicates the read end to `STDIN`, and sets relevant state information in `shell_state`. 
 * It then returns a special code (`2`) indicating the child.
 * 
 * The parent process closes the read end of the pipe, duplicates the write end to `STDOUT`, and 
 * sets relevant state information in `shell_state`, including the process ID of the child. It then 
 * returns a special code (`1`) indicating the parent.
 * 
 * Return:
 * - 2 if the current process is the child process.
 * - 1 if the current process is the parent process.
 */

// Pseudo Code for minipipe
// Create a pipe with two file descriptors: pipefd[0] for reading and pipefd[1] for writing.
// Fork the current process:
// If in the child process (pid == 0):
// Close the write end of the pipe (pipefd[1]).
// Duplicate the read end of the pipe (pipefd[0]) to STDIN.
// Assign the read end of the pipe to shell_state->pipe_intput_fd.
// Set shell_state->proc_id to -1.
// Set shell_state->is_parent_proc to 0.
// Reset the no_exec flag in shell_state.
// Return 2 to indicate this is the child process.
// If in the parent process:
// Close the read end of the pipe (pipefd[0]).
// Duplicate the write end of the pipe (pipefd[1]) to STDOUT.
// Assign the write end of the pipe to shell_state->pipe_output_fd.
// Set shell_state->proc_id to the child's process ID.
// Set shell_state->last_exit_status to 0.
// Return 1 to indicate this is the parent process.


// Main minipipe function
int	minipipe(t_shell_state *shell_state)
{
    int pipefd[2];
    pid_t pid;

    if (create_pipe(pipefd) == -1)
        return -1;

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return -1;
    }
    else if (pid == 0)
        return handle_child_process(pipefd, shell_state);
    else
        return handle_parent_process(pipefd, pid, shell_state);
}

//function to handle the operations "<" ">" ">>" and "|"
int ft_redir_iop(char *ops, t_shell_state *shell_state, t_cmd_token *token)
{
    int type = 0; // Initialize type (for redirection type, like TRUNC or APPEND)

    // Handle output redirection with ">" (truncate) and ">>" (append)
    if (ft_strcmp(ops, ">") == 0)
    {
        type = TRUNC;
        return redir(shell_state, token, type);
    }
    else if (ft_strcmp(ops, ">>") == 0)
    {
        type = APPEND;
        return redir(shell_state, token, type);
    }
    // Handle input redirection with "<"
    else if (ft_strcmp(ops, "<") == 0)
        return input(shell_state, token);
    // Handle piping with "|"
    else if (ft_strcmp(ops, "|") == 0)
        return minipipe(shell_state);
    else
    {
        // If the operation is not recognized, print an error
        ft_putendl_fd("Error: Invalid operation", STDERR);
        return -1;
    }
}