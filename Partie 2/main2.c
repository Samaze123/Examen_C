#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pid_t pid, childpid;
    int pipeToParent[2], fd, waitStatus, counterWhile = 1;
    char dataBuffer[2000] = "", pipeBuffer[2000] = "", *cmdLineStr, delimiter[2] = "\n";

    if((fd = open("data.txt", O_RDONLY)) < 0) // file opening
    {
        perror("open error ");
        exit(errno);
    }

    if((read(fd, dataBuffer, 2000)) < 0) // file reading
    {
        perror("read error ");
        exit(errno);
    }

    if((close(fd)) < 0)
    {
        perror("close open error ");
        exit(errno);
    }

    cmdLineStr = strtok(dataBuffer, delimiter); // strtok starting


    while (cmdLineStr != NULL) // while strtok returns NULL
    {

        if((pipe(pipeToParent)) < 0) // pipe creation
        {
            perror("pipe to parent error ");
            exit(errno);
        }

        if((pid = fork()) < 0) // fork creation
        {
            perror("fork error ");
            exit(errno);
        }


        if(pid)
        {
            // parent

            if((close(pipeToParent[1])) < 0) // parent close pipe writing
            {
                perror("parent close pipe writing error ");
                exit(errno);
            }

            if((wait(NULL)) < 0) // wait until child dead
            {
                perror("parent wait error ");
                exit(errno);
            }

            if((read(pipeToParent[0], pipeBuffer, 2000)) < 0)
            {
                perror("parent read error ");
                exit(errno);
            }

            if((close(pipeToParent[0])) < 0)
            {
                perror("close pipe reading error ");
                exit(errno);
            }

            printf("Results parent %d: \n%s\n\n\n\n", counterWhile, pipeBuffer);

            memset(pipeBuffer, 0, 2000);

            counterWhile++;
        }

        else
        {
            // child

            char *cmdArg, *cmdLineArray[200];
            int counterArg = 0;

            if((close(pipeToParent[0])) < 0)
            {
                perror("child close pipe reading error ");
                exit(errno);
            }

            if ((dup2(pipeToParent[1], STDERR_FILENO)) < 0)
            {
                perror("child dup stderr error ");
                exit(errno);
            }

            if ((dup2(pipeToParent[1], STDOUT_FILENO)) < 0)
            {
                perror("child dup stdout error ");
                exit(errno);
            }

            cmdArg = strtok(cmdLineStr, " ");

            //walk through other cmdArgs
            while( cmdArg != NULL ) 
            {
                cmdLineArray[counterArg] = malloc(strlen(cmdArg) + 1);

                strncpy(cmdLineArray[counterArg], cmdArg, strlen(cmdArg));
                                
                cmdArg = strtok(NULL, " ");

                counterArg++;
            }
            
            cmdLineArray[counterArg] = NULL;

            if(!(execvp(cmdLineArray[0], cmdLineArray)))
            {
                perror("Execlp child error ");
                exit(errno);
            }
            
            write(pipeToParent[1], "Bad command entered", strlen("Bad command entered"));

            if((close(pipeToParent[1])) < 0)
            {
                perror("child close pipe writing error ");
                exit(errno);
            }
            exit(1);
        }

        cmdLineStr = strtok(NULL, delimiter);
    }

    printf("closed \n");
    exit(0); // sucess
}
