#include <stdlib.h>
#include <stdio.h>

#define FILENAME "rttasks"
#define MAX_LINE_SIZE 256
#define MAX_NB_OF_TASKS 32
#define MAX_TASK_NAME_SIZE 32

#define ADMISSION 0
#define RUNNABLE 1
#define RUNNING 2
#define TERMINATED 3

typedef struct rttask {
  /* Initial elements */
  char name[MAX_TASK_NAME_SIZE];
  unsigned int arrival_time;      // Arrival Time
  unsigned int computation_time;  // Computation Time
  unsigned int relative_deadline; // Relative Deadline

  /* Used by scheduler */
  unsigned int state;             //
  unsigned int computations_done; // Number of computations done
  unsigned int num_executions ;   // Number of executions completed

} rttask;

char *states[] = {"admission", "runnable", "running", "terminated"};

char *get_state_string(int state) {
  return states[state];
}
void print_tasks(rttask tasks[], int num_tasks, int current_time) {
  int i;

  for(i = 1; i < num_tasks; i++) {
    printf("Task: %s  arrival_time:%d state:%s  computations:%d/%d time_to_deadline:%d\n",
	   tasks[i].name,
	   tasks[i].arrival_time,
	   get_state_string(tasks[i].state),
	   tasks[i].computations_done,
	   tasks[i].computation_time,
	   (tasks[i].arrival_time + (tasks[i].num_executions + 1) * tasks[i].relative_deadline) - current_time);
  }
}

/*
   Returns the index of the selected task
   Returns 0 if no task could be selected
   Returns the negative index of the task which has not met its deadline
*/
int EDF(rttask tasks[], int num_tasks, int current_time) {
  int i;

  // Admit new tasks based on the current_time and arrival_time
  for (i = 1; i < num_tasks; i++) {
    if (tasks[i].state == ADMISSION) {
      if (tasks[i].arrival_time == current_time) {
	tasks[i].state = RUNNABLE;
      }
    }
  }


  /*
     Foreach task,
       check if the deadline is expired
         check if the task is over:
         if yes, make it runnable
	 if not, error

   */
  for (i = 1; i < num_tasks; i++) {
    // Next deadline = Arrival Time + (Number of computations done + 1) * Relative deadline
    unsigned int next_deadline = tasks[i].arrival_time + (tasks[i].num_executions + 1) * tasks[i].relative_deadline;
    if (current_time == next_deadline) {
      // Check if the task is over
      tasks[i].state = RUNNABLE;
      tasks[i].computations_done = 0;
      tasks[i].num_executions++;
       if (tasks[i].state != ADMISSION) {
	 fprintf(stderr, "Task %d has not met its deadline\n", i);
       }
    }
  }
  


  /*
    Check if the current running process terminates
    if not make it runnable
   */
  for (i = 1; i < num_tasks; i++) {
    if (tasks[i].state == RUNNING) {
      if (tasks[i].computations_done == tasks[i].computation_time) {
	tasks[i].state = TERMINATED;
      }
    }
  }

  for (i = 1; i < num_tasks; i++){
    if (tasks[i].state == RUNNING) {
      tasks[i].state = RUNNABLE;
    }
  }
  // print_tasks(tasks,num_tasks,current_time);
  /*
    Check the tasks who has the lowest TTD (Time-To-Deadline)
  */
  int flag = 0;
  unsigned int task_to_schedule = -1;
  unsigned int lowest_time_to_deadline;
  for (i = 1; i < num_tasks; i++) {
    if (tasks[i].state == RUNNABLE && flag == 0) {
      lowest_time_to_deadline = (tasks[i].arrival_time + (tasks[i].num_executions + 1) * tasks[i].relative_deadline) - current_time;
      task_to_schedule = i;
      flag = 1;
    }
    if (tasks[i].state == RUNNABLE) {
      unsigned int time_to_deadline = (tasks[i].arrival_time + (tasks[i].num_executions + 1) * tasks[i].relative_deadline) - current_time;
      if (time_to_deadline < lowest_time_to_deadline) {
	    lowest_time_to_deadline = time_to_deadline;
	    task_to_schedule = i;
      }
    }
  }

  if ( task_to_schedule != -1 ) {
    tasks[task_to_schedule].computations_done++;
    tasks[task_to_schedule].state = RUNNING;
    return task_to_schedule;
  }



  // No task could be selected
  return 0;
}





int main(int argc, char *argv[]){
  char line [MAX_LINE_SIZE]; /* or other suitable maximum line size */
  rttask tasks[MAX_NB_OF_TASKS];
  int num_tasks = 1;
  int time = 0;
  int task_index;
  
  int step = 1;
  if (argc == 2){
    sscanf(argv[1], "%d", &step);  
  }


  /**** Reading the task file, and storing into a struct ****/
  FILE *file = fopen (FILENAME, "r" );
  if (file == NULL) {
    perror(FILENAME);
    return -1;
  }

  /* Reading the file line by line */
  printf("Loading file of tasks\n");
  while (fgets(line, sizeof line, file) != NULL ) {
    //printf("Line written=%s\n", line);
    sscanf(line, "%s %d %d %d",
	   tasks[num_tasks].name,
	   &tasks[num_tasks].computation_time,
	   &tasks[num_tasks].arrival_time,
	   &tasks[num_tasks].relative_deadline);

    tasks[num_tasks].state = ADMISSION;
    tasks[num_tasks].computations_done = 0;
    tasks[num_tasks].num_executions = 0;
    num_tasks++;

  }

  fclose(file);
  printf("%d tasks loaded\n\n", num_tasks - 1);

  print_tasks(tasks, num_tasks, time);

  /**** Scheduling the set of tasks ****/
  printf("Scheduling the set of tasks\n");


  while(1) {
    int i = 0;
    for (i = 0; i < step; i++) {
      //print_tasks(tasks, num_tasks, time);
      task_index = EDF(tasks, num_tasks, time);
      if (task_index > 0) {
	printf("******** Time %d: %s\n\n", time,  tasks[task_index].name);
      }
      else if (task_index == 0) {
	printf("******** Time %d: no task to schedule\n\n", time);
      }
      else {
	printf("Time %d: tasks %d has not met the deadline\n", time, -task_index);
      }
      time ++;
    }
    char quit = getchar();
    if (quit == 'q') return 0;
  }

}
