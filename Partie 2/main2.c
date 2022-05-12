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
    int pipeToParent[2], fd, waitStatus, counterWhile = 1, boolClosedPipeParent = 0;
    char dataBuffer[2000] = "", *cmdLineStr, delimiter[2] = "\n";

    if((fd = open(argv[1], O_RDONLY)) < 0) // file opening
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

    if((pipe(pipeToParent)) < 0) // pipe creation
    {
        perror("pipe to parent error ");
        exit(errno);
    }

    while (cmdLineStr != NULL) // while strtok returns NULL
    {

        if((pid = fork()) < 0) // fork creation
        {
            perror("fork error ");
            exit(errno);
        }

        if(pid)
        {
            // parent
            int waitRespons, status = 1, timeout = 2000, counter = 0, readRespons;
            char pipeBuffer[2000] = "";
            // if (boolClosedPipeParent == 0)
            // {
            //     if((close(pipeToParent[1])) < 0) //close pipe writing
            //     {
            //         perror("Close pipe writing parent error ");
            //         exit(errno);
            //     }
            //     boolClosedPipeParent = 1;
            // }

            while(WIFEXITED(status) == 0 && timeout > counter)
            {
                waitRespons = waitpid(pid, &status, WNOHANG);
                counter ++;
                sleep(0.1);
            }

            if (timeout <= counter)
            {
                kill(pid, SIGKILL);
            }

            if((readRespons = read(pipeToParent[0], pipeBuffer, 2000)) < 0)
            {
                perror("parent read error ");
                exit(errno);
            }

            printf("Results parent %d :\n%s \n\n\n\n", counterWhile, pipeBuffer);

            // while(readRespons != 0)
            // {
                // printf("%s\n", pipeBuffer);
            //     memset(pipeBuffer, 0, readRespons);
            //     printf("read respons : %d\n", readRespons);
            //     readRespons = read(pipeToParent[0], pipeBuffer, 2000);
                
                
            //     // if((readRespons = read(pipeToParent[0], pipeBuffer, 2000)) < 0)
            //     // {
            //     //     perror("parent read error ");
            //     //     exit(errno);
            //     // }
            // //     printf("ezeiuhezfiuhefziuhfze");
            //     sleep(1);
            // }
        }

        else
        {
            // child

            char *cmdArg, *cmdLineArray[200], delimiter[2] = "\n";
            int counterArg = 0;

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

            exit(1);
        }

        counterWhile++;
        cmdLineStr = strtok(NULL, delimiter);
    }

    printf("End of file \n");
    exit(0); // sucess
}
