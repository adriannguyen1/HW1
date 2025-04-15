#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMDLINE_MAX 512

int input = 1;
int output = 2;

//Struct Object
struct cmdline { //we're making a struct just like in hw1 instructions. It's gonna have an array to hold pointers to arguments
    //char command;
    char *arguments[256]; //we want an array of argument pointers the max number of arguments would be 256 (we got like 512 chars max so one char and one space per argument.)
};

//Function to parse
int parserfunction(struct cmdline *mycmd, char *cmdline) {
    char* token;

    token = strtok(cmdline, " \t");
    int i = 0;
    while (token != NULL) {
            mycmd->arguments[i] = token;
            token = strtok(NULL, " \t");
            
            if (strtok(token, "<") != NULL)
            {
                input = open();
            }
            
            i++;
    }
    mycmd->arguments[i] = NULL; //NULL terminate it. mark the end.

    return i;
}

//main of the code
int main(void)
{
    char cmd[CMDLINE_MAX];
    char *eof;
    pid_t pid;

    while (1) {
	    //char *args[CMDLINE_MAX];
        char *nl;
        //int retval;

        /* Print prompt */
        printf("sshell$ ");
        fflush(stdout);

        /* Get command line */
        eof = fgets(cmd, CMDLINE_MAX, stdin);
        if (!eof)
                /* Make EOF equate to exit */
                strncpy(cmd, "exit\n", CMDLINE_MAX);

        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) {
                printf("%s", cmd);
                fflush(stdout);
        }

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
                *nl = '\0';
        //skeleton code ends here.


        //Call of strok using struct. Edits start here!
        struct cmdline ourcmd; //make a struct to store our args
        struct cmdline *cmdptr = &ourcmd;

        char cmdCOPY[CMDLINE_MAX];
        strcpy(cmdCOPY, cmd); //gotta make a copy of cmd for printing out later, because cmd may be parsed


        int processNum = parserfunction(cmdptr, cmd);
        //call paserfunction!
        //processNum can now be used instead of original argi

        if (ourcmd.arguments[0] == NULL) {
            continue;
        }

		if (processNum > 16)
		{
			fprintf(stderr, "Error: too many process arguments\n");
			continue;
		}

        /* Builtin command */
        if (!strcmp(ourcmd.arguments[0], "exit")) {
                        
            fprintf(stderr, "Bye...\n");
            fprintf(stderr, "+ completed '%s' [%d]\n", cmdCOPY, 0);
            break;
        }

        /* Regular command */
        pid = fork();
        int status;

		if (pid == 0) // Child
		{
			execvp(ourcmd.arguments[0], ourcmd.arguments);
            perror("execvp"); // explain why it failed
			exit(1);
		}
		else if (pid > 0) // Parent
		{
			waitpid(pid, &status, 0);
			//retval = WEXITSTATUS(retval);
		}
		else
		{
			perror("fork");
			return 1;
		}

        int exitStatus;
        if (WIFEXITED(status)) {
        exitStatus = WEXITSTATUS(status);
        } else {
        exitStatus = 1; // Or another fallback value
        }

		if (exitStatus != 0)
			fprintf(stderr, "Error: command not found\n");

                fprintf(stderr, "+ completed '%s' [%d]\n",
                        cmdCOPY, exitStatus);

        input = 1;
        output = 2;
    }

    return EXIT_SUCCESS;
}