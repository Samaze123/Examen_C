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
    pid_t pid;
    char fileName[] = "data.txt";

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return EXIT_FAILURE;
    }


    if (!pid)
    {
        // child code
        int stdOutFD;
        

        stdOutFD = open(fileName, O_CREAT|O_WRONLY|O_TRUNC, 0666);
        
        if (stdOutFD == -1)
        {
            perror("Open child error");
            exit(errno);
        }



        dup2(stdOutFD, STDOUT_FILENO); // replace stdout
        // dup2(stdOutFD, STDERR_FILENO); // replace stdout
        close(stdOutFD);


        char **commandLine = malloc((argc + 1) * sizeof(char *));
        
        if (argc > 1)
        {
            commandLine[0] = argv[1];
            for (int i = 0; i < argc; ++i)
            {
                commandLine[i+1] = argv[i+2];
            }
            commandLine[argc]=NULL;
        }

        // for (int i = 0; i < argc-1; ++i)
        // {
        //     printf("argument %d : %s\n", i+1, commandLine[i]);
        // }


        execvp(commandLine[0], commandLine); 


    }
    else
    {
        // parent code
        int status, dataFP;
        char bufferData[PATH_MAX];
        
        wait(NULL);

        struct stat sysStat;

        if ((stat(fileName, &sysStat)) == -1) 
        {
            perror("stat error");
            exit(EXIT_FAILURE);
        }


        if ((dataFP = open(fileName, O_RDONLY)) == -1)
        {
            perror("Open parent error");
            exit(errno);
        }
        
        if ((read(dataFP, bufferData, sysStat.st_size)) == -1)
        {
            close (dataFP);
            perror("Read error");
            exit(errno);
        }
        close (dataFP);

        printf("Results :\n%s\n", bufferData);

        // while(read(dataFP, bufferData, PATH_MAX) > 0)
        // {
            // printf("%d : %s", counter, bufferData);
            // counter ++;
        // }

    }
    return 1;
}