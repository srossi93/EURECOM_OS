#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
  sleep(10);
  
  pid_t ret;
  char *name;

  ret = fork();
 
  if(ret < 0){
    printf("Error in process creation");
    return -1;
  }

  if (ret == 0) { /* Child */
    sleep(15);    
    name = "child";
  } else {
    name = "parent";
    kill(ret, SIGKILL);
  }



  printf("process %s ret  = %d\n", name, ret);

  return 0;
}
