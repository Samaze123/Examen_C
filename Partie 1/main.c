#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid;

    int pipeFD[2];

    if((pipe(pipeFD)) < 0) // create the pipe
    {
        perror("Pipe error ");
        exit(errno);
    }

    if ((pid = fork()) < 0) // do a fork
    {
        perror("fork error ");
        exit(errno);
    }

    if (!pid)
    {
        // child code        
       
        if((close(pipeFD[0])) < 0) // close pipe reading
        {
            perror("Close pipe reading child error ");
            exit(errno);
        }

        if(!(dup2(pipeFD[1], STDERR_FILENO))) // replace stderr
        {
            perror("Dup2 STDERR error ");
            exit(errno);
        } 
        if(!(dup2(pipeFD[1], STDOUT_FILENO))) // replace stdout
        {
            perror("Dup2 STDOUT error ");
            exit(errno);            
        } 

        if((close(pipeFD[1])) < 0) // close pipe writing 
        {
            perror("Close pipe writing child error ");
            exit(errno);
        }

        char *commandLine[] = { "/bin/sh", "-c", argv[1], 0 }; // create the command line
        

        if(!(execve(commandLine[0], commandLine, 0))) // execute the command line
        {
            perror("Execve child error ");
            exit(errno);
        } 
    }
    else
    {
        // parent code
        
        int respons, status = 1, timeout = 2000, counter = 0; 
        char bufferData[PATH_MAX];

        if((close(pipeFD[1])) < 0) // close pipe writing 
        {
            perror("Close pipe writing child error ");
            exit(errno);
        } 

        while(WIFEXITED(status) == 0 && timeout > counter) // wait for child with a time out
        {
            respons = waitpid(pid, &status, WNOHANG);
            counter ++;
            sleep(0.1);
        }

        if (timeout <= counter) // kill the child if he takes too much times
        {
            kill(pid, SIGKILL);
            printf("Child process time out\n");
        }

        int readRespons = read(pipeFD[0], bufferData, PATH_MAX); // read what the child wrote in the pipe
        
        if ((close (pipeFD[0])) == -1)
        {
            perror("Close pipe reading parent error ");
            exit(errno);
        }


        if (readRespons == -1) // print the read error if occured
        {
            perror("Read error ");
            exit(errno);
        }
        
        printf("Results : \n%s\n", bufferData); // print the results

    }
    return 1;
}
