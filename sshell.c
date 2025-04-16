#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMDLINE_MAX 512

enum ERRTYPE {
    NONE, NOIN, CANTIN, NOOUT, CANTOUT
}

//Struct Object
struct cmdline { //we're making a struct just like in hw1 instructions. It's gonna have an array to hold pointers to arguments
    //char command;
    char *arguments[256]; //we want an array of argument pointers the max number of arguments would be 256 (we got like 512 chars max so one char and one space per argument.)
};

// Fills processes with the processes from cmdline that were separated by pipes, returns number of processes
int pipefunction(char *cmdline, char processes[][CMDLINE_MAX]) {
    char *token = strtok(cmdline, "|");

    int i = 0;
    while (token != NULL) {
        strcpy(processes[i], token);
        token = strtok(NULL, "|");
        
        i++;
    }

    return i;
}

// Parse command, sets file descriptors as necessary
int parserfunction(struct cmdline *mycmd, char *cmdline, int *input, int *output, ERRTYPE *error) {
    char* token;

    token = strtok(cmdline, " \t");
    int i = 0;
    while (token != NULL) {
            mycmd->arguments[i] = token;
            token = strtok(NULL, " \t");
            
            char input_check[CMDLINE_MAX];
            char output_check[CMDLINE_MAX];
            strcpy(input_check, token);
            strcpy(output_check, token);
            if (strtok(input_check, "<") != NULL) {
                char *file = strtok(NULL, " \t");
                while (file[0] == ' ' || file[0] == '\t')
                    ++file;

                // No file passed in
                if (!strcmp(file, "")) {
                    *error = NOIN;
                    return -1;
                }

                *input = open(file, O_RDONLY);

                // Cannot open file
                if (*input == -1) {
                    *error = CANTIN;
                    return -1;
                }
            } else if (strtok(output_check, ">") != NULL) {
                char *file = strtok(NULL, " \t");
                while (file[0] == ' ' || file[0] == '\t')
                    ++file;

                // No file passed in
                if (!strcmp(file, "")) {
                    *error = NOOUT;
                    return -1;
                }

                *output = open(file, O_WRONLY);
                
                // Cannot open file
                if (*output == -1) {
                    *error = CANTOUT;
                    return -1;
                }
            }
            
            argumentNum++;
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
        char *nl;

        /* Print prompt */
        printf("sshell@ucd$ ");
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
        char cmdCOPY1[CMDLINE_MAX];
        strcpy(cmdCOPY, cmd); //gotta make a copy of cmd for printing out later, because cmd may be parsed

        char processes[CMDLINE_MAX][CMDLINE_MAX];
        int input = 1;
        int output = 2;

        int processNum = pipefunction(cmdCOPY1, processes);

        int argumentNums[CMDLINE_MAX];
        for (int i = 0; i < processNum; ++i) {
            ERRTYPE error = NONE;
            argumentNum[i] = parserfunction(cmdptr, cmd, &input, &output, &error);

            switch (error) {
                case NOIN:
                    fprintf(stderr, "Error: no input file\n");
                    continue;
                case NOOUT:
                    fprintf(stderr, "Error: no output file\n");
                    continue;
                case CANTIN:
                    fprintf(stderr, "Error: cannot open input file\n");
                    continue;
                case CANTOUT:
                    fprintf(stderr, "Error: cannot open output file\n");
                    continue;
            }

            // Redirection during piping, bad
            if (input != 1 && i != 0) {
                fprintf(stderr, "Error: mislocated input redirection\n");
                continue;
            } else if (output != 2 && i != processNum - 1) {
                fprintf(stderr, "Error: mislocated output redirection\n");
                continue;
            }
        }

        if (ourcmd.arguments[0] == NULL) {
            continue;
        }

        for (int i = 0; i < processNum; ++i) {
            if (argumentNums[i] > 16) {
                fprintf(stderr, "Error: too many process arguments\n");
                continue;
            }
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

