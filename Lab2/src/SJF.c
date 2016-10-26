#include <stdlib.h>
#include <stdio.h>

#define FILENAME "tasks"
#define MAX_LINE_SIZE 256
#define MAX_NB_OF_TASKS 32
#define MAX_TASK_NAME_SIZE 32

#define ADMISSION 0
#define RUNNABLE 1
#define RUNNING 2
#define TERMINATED 3

int highest_totalOfComputations = 0;

typedef struct task {
  /* Initial elements */
  char name[MAX_TASK_NAME_SIZE];
  unsigned int totalOfComputations;
  unsigned int arrivalDate;
  /* Used by scheduler */
  unsigned int state;
  unsigned int computationsDone;
} task;

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



/* Returns the index of the selected task */
/* Return -1 if no task could be selected */
int SJF(task tasks[], int nbOfTasks, int currentTime) {
  int i;

  // First admit task if currentTime >= arrivalTime
  for(i=0; i<nbOfTasks; i++) {
    if (tasks[i].state == ADMISSION) {
      if (tasks[i].arrivalDate == currentTime) {
	tasks[i].state = RUNNABLE;
      }
    }
  }

  // Is a task running? Has that running tasks finished its computations? 
  //   If yes, put it in terminated state
  //   If no, continue this task
  for(i=0; i<nbOfTasks; i++) {
    if (tasks[i].state == RUNNING) {
      if (tasks[i].computationsDone == tasks[i].totalOfComputations) {
	tasks[i].state = TERMINATED;
	break;
      } else {
	/* Select this task */
	tasks[i].computationsDone ++;
	return i;
      }
    }
  }


  // Otherwise, select a new runnable task
  int SJComputations = highest_totalOfComputations;
  int taskIndexToSchedule = -1;
  for (i = 0; i < nbOfTasks; i++) {
    if (tasks[i].state == RUNNABLE) {
      if (tasks[i].totalOfComputations <= SJComputations ) {
	  taskIndexToSchedule = i;
	  SJComputations = tasks[i].totalOfComputations;
      }
    }
  }

  if ( taskIndexToSchedule != -1 ) {
    tasks[taskIndexToSchedule].computationsDone ++;
    tasks[taskIndexToSchedule].state = RUNNING;
    return taskIndexToSchedule;
  }

  /* for(i=0; i<nbOfTasks; i++) {
    if (tasks[i].state == RUNNABLE) {
      if (tasks[i].computationsDone < tasks[i].totalOfComputations) {
	tasks[i].computationsDone ++;
	tasks[i].state = RUNNING;
	return i;
      }
    }
    }*/

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
    if (tasks[nbOfTasks].totalOfComputations > highest_totalOfComputations) 
      highest_totalOfComputations = tasks[nbOfTasks].totalOfComputations;
 
    nbOfTasks ++;
  
  }

  fclose(file);
  printf("%d tasks loaded\n\n", nbOfTasks);
  
  /**** Scheduling the set of tasks ****/
  printf("Scheduling the set of tasks\n");


  while(hasTasksToSchedule(tasks, nbOfTasks) > 0) {
    //    printTasks(tasks, nbOfTasks);
    taskIndex = SJF(tasks, nbOfTasks, time);
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
  

