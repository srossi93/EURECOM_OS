#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sched.h>


#define FINAL_VALUE 10

main(int argc, char * argv[]) {
  int        fd;
  unsigned  *addr;

  /* Create a new memory object */
  fd = shm_open("/mymem", O_RDWR | O_CREAT, 0777);
  if (fd == -1) {
    perror("Open failed");
    exit(0);
  }

  /* Set the memory object's size */
  if (ftruncate(fd, sizeof(*addr)) == -1) {
    perror("ftruncate error");
    exit(0);
  }

  /* Map the memory object */
  addr = (unsigned *)(mmap(0, sizeof(*addr), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (addr == MAP_FAILED) {
     perror("mmap failed");
     exit(0);
  }

  printf("Map addr is %6.6X\n", addr);

  // Remove the link to the memory
  close(fd);

  *addr = 0;
  if (fork()) {
    int i;
     for (i=0; i<FINAL_VALUE+1;)
       if (*addr == 0) {
	 printf("%d ->\n", i);
	 
	 // put value i at address addr
         *addr = i;

	 i++;
       } else
	 // call scheduler
         sched_yield();
  } else
    for (;;)
      if (*addr != 0) {
	 printf("%d <-\n", *addr);
	 if (*addr == FINAL_VALUE) {
	   exit(0);
	 }
         // put value 0 at address addr
	 *addr = 0;

      }else {
	// call scheduler
	sched_yield();

      }
}
