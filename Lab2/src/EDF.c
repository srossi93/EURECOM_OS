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
  unsigned int at; // Arrival Time
  unsigned int ct; // Computation Time
  unsigned int rd; // Relative Deadline
  
  /* Used by scheduler */
  unsigned int state; 
  unsigned int computations_done; //Flag to verify if current computation is done
  unsigned int num_executions ; // Number of executions completed 
  
} rttask;

char * states[] = {"admission", "runnable", "running", "terminated"};

char * getStateString(int state) {
  return states[state];
}

/* Returns the number of tasks that still have to be run, */
/* that is, all tasks which still have computations to perform */
int hasTasksToSchedule(task tasks[], int nbOfTasks) {
  int total = 0;
  int i;

  for(i=0; i<nbOfTasks; i++) {
    if (tasks[i].state != TERMINATED) {
      total ++;
    }
  }
  return total;
}
/* Returns the highest remaining time */
void cmp_highest_RT(task tasks[], int nbOfTasks) {
  int i;
  for(i=0; i<nbOfTasks; i++) {
    if (tasks[i].totalOfComputations - tasks[i].computationsDone > highest_RT) {
      highest_RT = tasks[i].totalOfComputations - tasks[i].computationsDone;
    }
  }
}

/* Returns the index of the selected task */
/* Returns -1 if no task could be selected */
/* Returns -2 if a task haven't met its deadline */ 
int EDF(task tasks[], int num_tasks, int current_time) {
  int i;

  // Admit new tasks based on the current_time and arrival_time
  for(i = 0; i < num_tasks; i++) {
    if (tasks[i].state == ADMISSION) {
      if (tasks[i].arrivalDate == currentTime) {
	tasks[i].state = RUNNABLE;
      }
    }
  }
  
  // If the deadline is expired, check if the task is over

  // Choose which task should run
  // It's the one which has the lowest TTD (Time-To-Deadline) 

  
  for(i=0; i<nbOfTasks; i++) {
    if (tasks[i].state == RUNNING) {
      if (tasks[i].computationsDone == tasks[i].totalOfComputations) {
	tasks[i].state = TERMINATED;
	break;
      } else {
	/* Make it runnable */
	tasks[i].state = RUNNABLE;
      }
    }
  }


  // Otherwise, select a new runnable task
  cmp_highest_RT(tasks, nbOfTasks);
  int RTComputations = highest_RT;
  int taskIndexToSchedule = -1;
  for (i = 0; i < nbOfTasks; i++) {
    if (tasks[i].state == RUNNABLE) {
      if (tasks[i].totalOfComputations - tasks[i].computationsDone <= RTComputations ) {
	  taskIndexToSchedule = i;
	  RTComputations = tasks[i].totalOfComputations - tasks[i].computationsDone;
      }
    }
  }

  if ( taskIndexToSchedule != -1 ) {
    tasks[taskIndexToSchedule].computationsDone ++;
    tasks[taskIndexToSchedule].state = RUNNING;
    return taskIndexToSchedule;
  }



  // No task could be selected
  return -1;
}


void printTasks(task tasks[], int nbOfTasks) {
  int i;

  for(i=0; i<nbOfTasks; i++) {
    printf("Task: %s  arrivalDate:%d state:%s  computations:%d/%d\n", 
	   tasks[i].name, 
	   tasks[i].arrivalDate, 
	   getStateString(tasks[i].state), 
	   tasks[i].computationsDone, 
	   tasks[i].totalOfComputations);
  }
}


int main(int argc, char *argv[]){
  char line [MAX_LINE_SIZE]; /* or other suitable maximum line size */
  task tasks[MAX_NB_OF_TASKS];
  int nbOfTasks = 0;
  int time = 0;
  int taskIndex;

 

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
    sscanf(line, "%s %d %d", 
	   tasks[nbOfTasks].name, 
	   &tasks[nbOfTasks].totalOfComputations, 
	   &tasks[nbOfTasks].arrivalDate);
    
    tasks[nbOfTasks].state = ADMISSION;
    tasks[nbOfTasks].computationsDone = 0; 

    // Update the longest job
    cmp_highest_RT(tasks, nbOfTasks);
 
    nbOfTasks ++;
  
  }

  fclose(file);
  printf("%d tasks loaded\n\n", nbOfTasks);
  
  /**** Scheduling the set of tasks ****/
  printf("Scheduling the set of tasks\n");


  while(hasTasksToSchedule(tasks, nbOfTasks) > 0) {
    //    printTasks(tasks, nbOfTasks);
    taskIndex = SRTN(tasks, nbOfTasks, time);
    if (taskIndex >= 0) {
      printf("Time %d: %s\n", time,  tasks[taskIndex].name);
    } else {
      printf("Time %d: no task to schedule\n", time);
    }
    time ++;
  }


  /**** That's all folks ****/
  //  printTasks(tasks, nbOfTasks);
  time --;
  printf("All done after %d units of time\n", time);
  return 0;
}
  

