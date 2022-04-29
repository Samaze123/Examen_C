#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <string.h>
#include<sys/stat.h>
#include <fcntl.h> 

int main(int argc, char const *argv[])
{

  int forkingIndex, pCloseStatus;
  int counterResults = 1;
  int dataFileChild;
  FILE *pointerCommandFile, *pointerDataFile;
  char dataLine[PATH_MAX], commandLine[PATH_MAX];

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
    // sleep(3);

    // pointerDataFile = fopen("data.log", "r");

    // while(fgets(dataLine, PATH_MAX, pointerDataFile) != NULL)
    // {

    //   printf("%d : %s\n", counterResults, dataLine);
    //   counterResults ++;
    // }

    // fclose(pointerDataFile);

  }

  /* ------------- Child ---------------------*/ 
  else 
  {  


    // In the child
    // for (int i = 0; i < argc; ++i)
    // {
    //   printf("%d", i);
    // }
    
    printf("%d", argc);
    
  // execlp(argv[1], argv[1], NULL);
    
    printf("c\n");

    dataFileChild = creat("data.log", O_RDWR);

    printf("a\n");

    // while (read(pointerCommandFile, dataLine, PATH_MAX) != 0)
    // {
    //   printf("%s\n", dataLine);
    // }

    printf("b\n");


    
  }
  return 0;
}
