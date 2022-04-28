#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <string.h>

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}







int main(int argc, char const *argv[])
{

  int processIdForking, pCloseStatus, status;
  int counterResults = 1;
  FILE *pointerStreamPopen;
  char dataLine[PATH_MAX];
  char commandLine[PATH_MAX];
  char strfgets[PATH_MAX];

  /* ----------- Forking a child -------------*/

  if ( (processIdForking = fork() ) ) 
  {

    if (processIdForking < 0) 
    {

      perror("Process cannot be forked");
      exit(errno);
    }

    /* ----------- Parent --------------------*/
    // sleep(5);
    


    // if ((processIdForking = waitpid(processIdForking, &status, WNOHANG)) == -1)
    //   perror("wait() error");
    // else if (processIdForking == 0) 
    // {
    //   printf("child is still running at");
    //   sleep(1);
    // }
    


    // while(fgets(dataLine, PATH_MAX, pointerDataFile) != NULL)
    // {

    //   printf("%s\n", dataLine);
    // }
  }

  /* ------------- Child ---------------------*/ 
  else 
  { 
    // In the child (fork = 0)
    strcat(commandLine, argv[1]);
    strcat(commandLine, " 2>&1");

    printf("a\n");

    pointerStreamPopen = popen(commandLine, "r");
    
    printf("b\n");    

    if (pointerStreamPopen == NULL)
    {

        perror("Error on popen ");
        exit(errno);
    }    
    
    while(fgets(dataLine, PATH_MAX, pointerStreamPopen) != NULL)
    {

      printf("Resultat : %s\n", dataLine);
    }
    printf("c\n");

    pCloseStatus = pclose(pointerStreamPopen);
    
    if (pCloseStatus == -1) 
    {

      // Error reported by pclose()
      perror("Error on pclose : ");
      exit(errno);
    }    
  }
  /*-------------------Exit the fork--------------*/
  return 0;
}
