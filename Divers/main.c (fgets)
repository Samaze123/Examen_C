#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <string.h>

int main(int argc, char const *argv[])
{

  int forkingIndex, pCloseStatus;
  int boolErrorOnCmd = 0;
  int counterResults = 1;
  FILE *pointerCommandFile, *pointerDataFile;
  char dataLine[PATH_MAX];
  char commandLine[PATH_MAX];

  /* ----------- Forking a child -------------*/

  if ( (forkingIndex = fork() ) ) 
  {

    if (forkingIndex < 0) 
    {

      perror("Process cannot be forked");
      exit(errno);
    }

    /* ----------- Parent --------------------*/
    wait(NULL);
    pointerDataFile = fopen("data.log", "r");

    while(fgets(dataLine, PATH_MAX, pointerDataFile) != NULL)
    {

      printf("%d : %s\n", counterResults, dataLine);
      counterResults ++;
    }
    fclose(pointerDataFile);
  }

  /* ------------- Child ---------------------*/ 
  else 
  {  
    // In the child
    strcat(commandLine, argv[1]);
    strcat(commandLine, " >data.log 2>&1");

    pointerCommandFile = popen(commandLine, "r");

    if (pointerCommandFile == NULL)
    {

        perror("Error on popen ");
        exit(errno);

    }

    pCloseStatus = pclose(pointerCommandFile);
    
    if (pCloseStatus == -1) 
    {

      // Error reported by pclose()
      perror("Error on pclose : ");
      exit(errno);
    }
  }
  return 0;
}
