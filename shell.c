/*
* File Name: shell.c
* Assignment: Lab 3
* Completed by: Sudarshan Naicker, Alessandro Baldassarre
* Submission Date: Oct 04, 2023
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "parser.h"

#define BUFLEN 1024


int main()
{
    char buffer[1024];
    char *parsedinput;
    char *args[64] = {NULL};
    char *args2[64] = {NULL};
    char newline;
    int status;
    

    printf("Welcome to the Group23 shell! Enter commands, enter 'quit' to exit\n");
    
    do
    {

        // Remove all terminated child processes
        while (1)
        {
            pid_t trmtd_pid = waitpid(-1, &status, WNOHANG);
            if (trmtd_pid > 0)
            {
                printf("\nBackground command %d terminated.\n", trmtd_pid);
            }
            else
            {
                break;
            }
        }

        // Print the terminal prompt and get input
        printf("$ ");
        char *input = fgets(buffer, sizeof(buffer), stdin);
        if (!input)
        {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }

        // Clean and parse the input string
        parsedinput = (char*) malloc(BUFLEN * sizeof(char));
        size_t parselength = trimstring(parsedinput, input, BUFLEN);

        // Check for background execution
        bool checkBackground = runinbackground(parsedinput, parselength);
        if (checkBackground)
        {
            parsedinput[parselength - 1] = '\0'; // Remove the '&' operator from the input
        }

        // Sample shell logic implementation
        if (strcmp(parsedinput, "quit") == 0)
        {
            printf("Bye!!\n");
            free(parsedinput);
            return 0;
        }
        else
        {
            int pipePos = findpipe(parsedinput, BUFLEN);
            if (pipePos >= 0) {
                parsedinput[pipePos] = '\0';  //spliting the before and after pipe
                
                // Setting up the pipe
                int pipe_fd[2];
                if (pipe(pipe_fd) == -1) {
                    perror("Failed to create pipe");
                    return -1;
                }

                //..............tokenize first command...............
                tokenize_command(parsedinput, args);

                pid_t child1 = fork();//child 1 for first command (before '|')
                if (child1 == 0) {
                    
                    close(pipe_fd[0]);  // Close the read end.
                    dup2(pipe_fd[1], STDOUT_FILENO);  // Redirect stdout to the write end of the pipe
                    close(pipe_fd[1]); // Close the write end.

                    if (execvp(args[0], args) == -1) {
                        fprintf(stderr, "Error running command in execve\n");
                        return -100;
                    }
                } 
                else {

                    close(pipe_fd[1]);  // Close the write end.
                    waitpid(child1, NULL, 0);  // Wait for the first child to finish.


                    // ..........tokenize second command..............
                    tokenize_command(parsedinput + pipePos + 1, args2);

                    pid_t child2 = fork(); //child 2 for second command (after '|')
                    if (child2 == 0) {

                        close(pipe_fd[1]);// Wait for the first child to write.
                        dup2(pipe_fd[0], STDIN_FILENO);// Redirect stdin to read from the pipe.
                        close(pipe_fd[0]);
                        
                        if (execvp(args2[0], args2) == -1) {
                            fprintf(stderr, "Error running command in execve\n");
                            return -100;
                        }
                    }
                    else {
                        close(pipe_fd[0]);  // Close the write end.
                        waitpid(child2, NULL, 0); // Wait for the second child to finish.
                    
                    }
                    
                }
            }   
            else{

                tokenize_command(parsedinput, args);

                if (args[0][0] != '/' && strchr(args[0], '/') == NULL)
                {
                    // find the command in PATH.
                    char *command_path = find_command_in_path(args[0]);
                    if (command_path)
                    {
                        args[0] = command_path; // Update the command to the full path found
                    }
                    else
                    {
                        fprintf(stderr, "Command not found\n");
                        free(parsedinput);
                        continue; // Jump to the next loop iteration
                    }
                }
                
                pid_t forkV = fork();
                if (forkV == 0){

                    if (execve(args[0], args, NULL) == -1)
                    {
                        fprintf(stderr, "Error running command in execve\n");
                        return -100;
                    }
                }
                else{

                    if (!checkBackground){
                        
                        waitpid(forkV, NULL, 0);
                    }
                    else{

                        printf("Background command %d started.\n", forkV);
                    }
                }
            }
        }

        // Remember to free any memory you allocate!
        for (int i = 0; i < 64; i++) {
            args[i] = NULL;
            args2[i] = NULL;
        }
        printf("\n");
        

        free(parsedinput);
    } while (1);

    return 0;
}
