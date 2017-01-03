#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sched.h>
#include <pthread.h>


#define FINAL_VALUE 10

void* writer(void*);
void* reader(void*);


unsigned  *addr;
pthread_mutex_t mutex_write;
pthread_mutex_t mutex_read;
pthread_cond_t cv;

main(int argc, char * argv[]) {
  int fd;

  pthread_t reader_tid, writer_tid;

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

  pthread_mutex_init(&mutex_write, NULL);
  pthread_mutex_init(&mutex_read, NULL);
  pthread_mutex_lock(&mutex_read);

  pthread_cond_init(&cv, NULL);
 
  pthread_create(&writer_tid, NULL, writer, NULL);
  pthread_create(&reader_tid, NULL, reader, NULL);

  pthread_join(writer_tid, NULL);
  pthread_join(reader_tid, NULL);

}
 
void* writer(void* argv) { 
  int i;
  for (i = 0; i < FINAL_VALUE + 1;) {
    printf("%d ->\n", i);
    
    // -------> CS
    pthread_mutex_lock(&mutex_write);
    *addr = i;
    //pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mutex_read);
    // <------- CS

    i++; 
  }
  pthread_exit(0);
}


void* reader(void* argv) {
  for (;;) {
    pthread_mutex_lock(&mutex_read);
    //pthread_cond_wait(&cv, &mutex_read);
 
    printf("%d <-\n", *addr);
    if (*addr == FINAL_VALUE) {
      pthread_exit(0);
    }
    pthread_mutex_unlock(&mutex_write);
  }
}
