#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define MAX_MSG 15
#define OVER 37
#define MQ_NAME "/mqname" /* Message queue name: To be modified. The name */
                          /* must start with a "/" */
#define MQ_SIZE 10
#define PMODE 0666


/* to open the message queue */
void init_queue (mqd_t *mq_desc, char *mq_name, int open_flags) {
  struct mq_attr attr;
  
  // fill attributes of the mq
  attr.mq_maxmsg = MQ_SIZE;
  attr.mq_msgsize = sizeof (int);
  attr.mq_flags = 0;

  // open the mq
  //*mq_desc = mq_open (MQ_NAME, open_flags);
  *mq_desc = mq_open (mq_name, open_flags, PMODE, &attr);
  if (*mq_desc == (mqd_t)-1) {
    perror("Mq opening failed");
    exit(-1);
  }
}


/* to add an integer to the message queue */
void put_integer_in_mq (mqd_t mq_desc, int data) {
  int status;
  
  //sends message
  status = mq_send (mq_desc, (char *) &data, sizeof (int), 1);
  if (status == -1)
    perror ("mq_send failure");
}


/* to get an integer from message queue */
int get_integer_from_mq (mqd_t mq_desc) {
  ssize_t num_bytes_received = 0;
  int data=0;

  //receive an int from mq
  num_bytes_received = mq_receive (mq_desc, (char *) &data, sizeof (int), NULL);
  if (num_bytes_received == -1)
    perror ("mq_receive failure");
  return (data);
}


/* code of a sending process */
void sender (char *name)
{
  int n;
  mqd_t mqfd;

  //printf("I am in sender\n");
  init_queue (&mqfd, name, O_CREAT | O_WRONLY);
  for (n = 0; n < MAX_MSG; n++) {
      printf ("%d ?--->\n", n);
      put_integer_in_mq (mqfd, n);
    }
  put_integer_in_mq (mqfd, OVER);
  
  /* close queue */
  mq_close (mqfd);
  return ;
}

/* code of receiving process */
void receiver (char *name)
{
  int d;
  mqd_t mqfd;

  // printf("I am in receiver\n");
  init_queue (&mqfd, name, O_CREAT | O_RDONLY);
  while (1) {
      d = get_integer_from_mq (mqfd);
      if (d == OVER)
	break;
      printf ("---> %d\n", d);
    }
  /* Close queue */
  mq_close(mqfd);

  return ;
}

void son1(void);
void son2(void);

void son11(void);
void son12(void);

int main(void) {
  pid_t ret1, ret2;
  
  ret1 = fork();
  if (ret1 < 0)
    perror("Error in process creation: SON1 cannot be created");
  else if (ret1 == 0)
    son1();
  else {
      ret2 = fork();
      if (ret2 < 0)
	perror("Error in process creation: SON2 cannot be created");
      else if (ret2 == 0)
	son2();
      else {
	int i;
	for (i = 0; i < 2; i++)
	  wait(NULL);
      }
  }

  return(0);  
}

void son1(void) {
  pid_t ret11, ret12;
  
  ret11 = fork();
  if (ret11 < 0)
    perror("Error in process creation: SON11 cannot be created");
  else if (ret11 == 0)
    son11();
  else {
     ret12 = fork();
     if (ret12 < 0)
       perror("Error in process creation: SON12 cannot be created");
     else if (ret12 == 0)
       son12();
     else {
       int i;
       for (i = 0; i < 2; i++)
	 wait(NULL);
       exit(0);
     }
  }
}

void son2(void) {
  int data_from_s11;
  mqd_t mq11_2, mq2_12;

  init_queue (&mq2_12, "/mq2_12", O_CREAT | O_WRONLY);
  init_queue (&mq11_2, "/mq11_2", O_CREAT | O_RDONLY);

   while (1) {
      data_from_s11 = get_integer_from_mq (mq11_2);
      put_integer_in_mq(mq2_12, data_from_s11);
      
      if (data_from_s11 == OVER)
	break;
      
      printf("---> %d --->\n", data_from_s11);
   }

  /* close queue */
   //mq_close (mqfd);
   exit(0);
}


void son11(void) {
  sender("/mq11_2");
  exit(0);
}


void son12(void) {
  receiver("/mq2_12");
  mq_unlink("/mq2_12");
  exit(0);
}
