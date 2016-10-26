#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define MAX 3
#define SPL 10
int main(void) {
  int i;
  int j;

  for (i = 0; i < MAX; i++)
  {
    pid_t ret = fork();
    if (ret == 0)
    {
      if ( i == 0 || i == MAX - 1 )
      {
        for (j = 0; j < 2; j++)
        {
          pid_t ret2 = fork();
          if (ret2 == 0)
          {
	    printf("%d", j);
            sleep(SPL);
            exit(0);    
          }
        }
        printf("%d/n", i);
        sleep(SPL);
        exit(0);    
      }
    }  
  }
  sleep(200);
  printf("Parent");
  return(0);
}
