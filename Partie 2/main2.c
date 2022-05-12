#define SIZE_OF_BUFFER 5000


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
    // For each arguments
    for (int fileCounter = 1; fileCounter < argc; ++fileCounter)
    {   
        pid_t pid;
        int pipeToParent[2], fileDescrip, commandCounter = 1;
        char dataBuffer[SIZE_OF_BUFFER] = "", *cmdLineStr, delimiter[2] = "\n";

        // Open the file
        if((fileDescrip = open(argv[fileCounter], O_RDONLY)) < 0) // file opening
        {
            perror("Open file error ");
            exit(errno);
        }

        // Read the file
        if((read(fileDescrip, dataBuffer, SIZE_OF_BUFFER)) < 0) // file reading
        {
            perror("Read file error ");
            exit(errno);
        }

        // Close the file
        if((close(fileDescrip)) < 0)
        {
            perror("Close file error ");
            exit(errno);
        }

        // Get the first command
        cmdLineStr = strtok(dataBuffer, delimiter); // strtok starting

        // Get each commands and execute them
        while (cmdLineStr != NULL) // while strtok returns NULL
        {

            // Create a pipe --> [0] for reading || [1] for writing
            if((pipe(pipeToParent)) < 0) // pipe creation
            {
                perror("pipe to parent error ");
                exit(errno);
            }

            // Create a child
            if((pid = fork()) < 0) // fork creation
            {
                perror("fork error ");
                exit(errno);
            }

            // Code of the parent
            if(pid)
            {
                int waitRespons, waitStatus = 1, timeout = 2000, counter = 0, readResponsParent = 1;
                
                // Wait until the child dies or the timeout expire
                while(WIFEXITED(waitStatus) == 0 && timeout > counter)
                {
                    waitRespons = waitpid(pid, &waitStatus, WNOHANG);
                    counter ++;
                    sleep(0.1);
                }

                // If the timeout expire --> the child is killed
                if (timeout <= counter)
                {
                    kill(pid, SIGKILL);
                }

                // Close the writing in the pipe for the parent
                if((close(pipeToParent[1])) < 0)
                {
                    perror("Parent close writing pipe error ");
                    exit(errno);
                }
                
                // Print the counter of the file and the counter of the command
                printf("\nResults parent %d|%d :\n", fileCounter, commandCounter);

                // Read the entire response
                while(readResponsParent != 0)
                {
                    char pipeBuffer[SIZE_OF_BUFFER] = "";
                    
                    // Read the informations in the pipe
                    if((readResponsParent = read(pipeToParent[0], pipeBuffer, SIZE_OF_BUFFER)) < 0)
                    {
                        perror("Parent read error ");
                        exit(errno);
                    }
                    
                    // Print the informations read
                    printf("%s", pipeBuffer);
                }
            }
            // Code of the child
            else
            {
                char *cmdArg, *cmdLineArray[200], delimiter[2] = "\n";
                int counterArg = 0;

                // Redirect the errors to the pipe
                if ((dup2(pipeToParent[1], STDERR_FILENO)) < 0)
                {
                    perror("child dup stderr error ");
                    exit(errno);
                }

                // Redirect the output to the pipe
                if ((dup2(pipeToParent[1], STDOUT_FILENO)) < 0)
                {
                    perror("child dup stdout error ");
                    exit(errno);
                }

                // Get the first word of the command
                cmdArg = strtok(cmdLineStr, " ");

                // Get all the word of the command
                while( cmdArg != NULL ) 
                {
                    // Create space in memory of the size of the word
                    cmdLineArray[counterArg] = malloc(strlen(cmdArg) + 1);

                    // Move the word into the created memory
                    strncpy(cmdLineArray[counterArg], cmdArg, strlen(cmdArg));
                    
                    cmdArg = strtok(NULL, " ");

                    counterArg++;
                }
                
                // The last word of the command is NULL
                cmdLineArray[counterArg] = NULL;

                // Execute the command 
                if(!(execvp(cmdLineArray[0], cmdLineArray)))
                {
                    perror("Execlp child error ");
                    exit(errno);
                }
                
                // Write in the pipe an error if the command wasn't executed
                write(pipeToParent[1], "Bad command entered", strlen("Bad command entered"));

                // Exit the child with 1 --> the child didn't run his code successfully
                exit(1);
            }

            // We go to the next command
            commandCounter++;
            cmdLineStr = strtok(NULL, delimiter);
        }
    }
    // The programme execute properly and a message is printed on the screen
    printf("\nEnd of file \n");
    exit(0); // sucess
}
