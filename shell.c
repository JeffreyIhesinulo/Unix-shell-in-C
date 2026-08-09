#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 1024
#define MAX_ARGS 64
#define PROMPT "mush>"

int main(void)
{
    char line[MAX_LINE_LENGTH];   // buffer to store the raw input line
    char* args[MAX_ARGS];         // array of pointers to parsed arguments (tokens)
    pid_t pid;                    // stores the process ID returned by fork()
    int status;                   // stores the exit status of the child process

    while (1)
    {
        printf("%s", PROMPT);
        fflush(stdout);   // force the prompt to display immediately (no newline printed yet)

        // Read a line of input; if EOF (Ctrl+D) is received, fgets returns NULL
        if (fgets(line, MAX_LINE_LENGTH, stdin) == NULL)
        {
            printf("\n");
            break;   // exit the shell on EOF
        }

        // Remove the trailing newline character left by fgets
        line[strcspn(line, "\n")] = '\0';

        // Skip empty input (user just pressed Enter)
        if (strlen(line) == 0)
        {
            continue;
        }

        // Tokenize the line into individual arguments, split by spaces
        int i = 0;
        char *token = strtok(line, " ");

        while (token != NULL && i < MAX_ARGS - 1)
        {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;   // execvp requires the argument array to be NULL-terminated

        // Guard against input that was only whitespace (no real tokens produced)
        if (args[0] == NULL)
        {
            continue;
        }

        // Built-in command: exit the shell
        if (strcmp(args[0], "exit") == 0)
        {
            printf("Bye!\n");
            break;
        }

        // Built-in command: change directory
        // Must be handled in the parent process directly (not via fork/exec),
        // since a child process changing its own directory has no effect on the shell itself
        if (strcmp(args[0], "cd") == 0)
        {
            if (args[1] == NULL)
            {
                // No argument given -> default to the HOME directory
                args[1] = getenv("HOME");
                if (args[1] == NULL)
                {
                    fprintf(stderr, "cd: HOME variable in not set\n");
                    continue;
                }
            }

            if (chdir(args[1]) != 0)
            {
                perror("cd");   // print system error message (e.g. "No such file or directory")
            }
            continue;
        }

        // Built-in command: print current working directory
        if (strcmp(args[0], "pwd") == 0)
        {
            char cwd[1024];

            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s\n", cwd);
            }
            else
            {
                perror("pwd");
            }
            continue;
        }

        // Built-in command: show available commands
        if (strcmp(args[0], "help") == 0)
        {
            printf("Available commands:\n");
            printf("    cd [directory]  - change directory\n");
            printf("    pwd             - show current direcotry\n");
            printf("    exit            - exit the shell\n");
            printf("    help            - this manual\n");
            continue;
        }

        // Not a built-in command -> create a child process to run an external program
        pid = fork();

        if (pid < 0)
        {
            perror("fork");   // fork failed (rare, e.g. out of resources)
            continue;
        }

        if (pid == 0)
        {
            // Child process: replace itself with the requested program
            // execvp searches the PATH environment variable to locate the executable
            execvp(args[0], args);

            // execvp only returns if it failed to find/run the command
            fprintf(stderr, "%s: command not found\n", args[0]);
            exit(EXIT_FAILURE);
        }
        else
        {
            // Parent process: wait for the child to finish before showing the prompt again
            wait(&status);

            if (WIFEXITED(status))
            {
                int exit_status = WEXITSTATUS(status);
                // exit_status currently unused, but available for future error handling/logging
            }
            else
            {
                printf("Unexpected programm shutdown\n");
            }
        }
    }
    return 0;
}