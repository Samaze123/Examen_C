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
    
    // If the file is accessible
    printf("%d", boolErrorOnCmd);

    if (boolErrorOnCmd == 1)
    {

      if (access ("err.log", F_OK) != -1)
      {

        pointerDataFile = fopen("err.log", "r");

        while(fgets(dataLine, PATH_MAX, pointerDataFile) != NULL)
        {

          printf("%s\n", dataLine);
        }
        fclose(pointerDataFile);
      }

      else
      {

        perror("Error Description Opening err.log ");
        exit(errno);
      }
    }

    // If the file is accessible

    else if (access ("data.log", F_OK) != -1)
    {

      pointerDataFile = fopen("data.log", "r");

      while(fgets(dataLine, PATH_MAX, pointerDataFile) != NULL)
      {

        printf("%d : %s\n", counterResults, dataLine);
        counterResults ++;
      }
    }
    else
    {

      perror("Error Description Opening data.log ");
      exit(errno);
    }
  }

  /* ------------- Child ---------------------*/ 
  else 
  {  
    // In the child
    strcat(commandLine, argv[1]);
    strcat(commandLine, " >data.log 2>err.log");

    pointerCommandFile = popen(commandLine, "r");

    if (pointerCommandFile == NULL)
    {

        perror("Error on popen ");
        boolErrorOnCmd = 1;
        // exit(errno);

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
