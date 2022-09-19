// clang -lpthread -std=gnu11 main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#define timeUnitUs 100 * 1000 // about 100 ms

pthread_mutex_t taskStateMutex;

enum taskState {idle, running, preempted, finished};
const char *taskStateString[4] = {"idle", "running", "preempted", "finished"};

struct Task {
	enum taskState state;
	int ID;
	int startTime; // In some imaginary integer time unit
	int totalRuntime; // In some imaginary integer time unit
	int currentRuntime; // In some imaginary integer time unit
};


void set_task_state(struct Task * task, enum taskState taskNewState){
	pthread_mutex_lock(&taskStateMutex);
	task->state = taskNewState;
	pthread_mutex_unlock(&taskStateMutex);
}

void *task_handler(void *var) {
	struct Task * task;
	task = (struct Task *) var;
	enum taskState prevTaskState = task->state;
	printf("Task handler initiated for task with ID %d in state %s \n", task->ID, taskStateString[task->state]);
	fflush(stdout);
	while(task->currentRuntime < task->totalRuntime){
		if(task->state != prevTaskState){
			enum taskState taskOldState = prevTaskState;
			prevTaskState = task->state;
			printf("Task with ID %d changed state from %s to %s after having worked for %d time units \n", task->ID, taskStateString[taskOldState], taskStateString[task->state], task->currentRuntime);
			fflush(stdout);
		}
		if(task->state == running){
			task->currentRuntime ++;
		}
		usleep(timeUnitUs);
	}
	printf("Task with ID %d finising after having worked for %d out of %d time units \n", task->ID, task->currentRuntime, task->totalRuntime);
	fflush(stdout);
	set_task_state(task, finished);
	return NULL;
}


void wait_for_rescheduling(int tick_time_units, struct Task* task){
	int runtime = 0;
	while(task->state != finished && runtime < tick_time_units){
		usleep(timeUnitUs);
		runtime++;
	}
}

void round_robin(struct Task ** tasks, int taskCount, int timeout, int interval){
	printf("Started RR scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		
		counter = 0;
		while(counter < taskCount){
			currentTaskIndex = (currentTaskIndex + 1) % taskCount;
			if(tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed){
				break;
			}
			counter ++;
		}

		if(counter >= taskCount){
			totalTimePassed ++; //Speeding through areas without tasks to schedule
		} else {
			if(prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished){ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}

// Implement your schedulers here!
void first_come_first(struct Task ** tasks, int taskCount, int timeout, int interval){
	printf("Started FCFS scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	// int currentTaskIndex = 0;
	// int prevTaskIndex = -1;
	int tasksCompleted = 0;
	int currentTaskStartTime = -1;
	int currentTaskIndex = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		for(int i = 0; i < taskCount; i++){
			if((tasks[i] ->startTime < currentTaskStartTime || currentTaskStartTime == -1) && tasks[i] -> state != finished){
				currentTaskIndex = i;
				currentTaskStartTime = tasks[i] -> startTime;
			}
		}
		// counter = 0;
		// while(counter < taskCount){
		// 	currentTaskIndex = (currentTaskIndex + 1) % taskCount;
		// 	if(tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed){
		// 		break;
		// 	}
		// 	counter ++;
		// }

		// if(counter >= taskCount){
		// 	totalTimePassed ++; //Speeding through areas without tasks to schedule
		// } else {
		// 	if(prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished){ // prevTaskIndex == -1 means that this is the first task to be selected
		// 		set_task_state(tasks[prevTaskIndex], preempted);
		// 	}
		set_task_state(tasks[currentTaskIndex], running);
		wait_for_rescheduling(tasks[currentTaskIndex] ->totalRuntime, tasks[currentTaskIndex]);
		set_task_state(tasks[currentTaskIndex], finished);

		totalTimePassed += (tasks[currentTaskIndex]->currentRuntime);

		if(tasksCompleted >= taskCount || totalTimePassed >= timeout){

		}
		// }
	}
}

void shortest_process_next(struct Task ** tasks, int taskCount, int timeout, int interval){

}

void highest_Rr_next(struct Task ** tasks, int taskCount, int timeout, int interval){

}




int main(){
	printf("Main started \n");
	fflush(stdout);
	int taskCount = 5;
	pthread_t threads[taskCount];
	
	// {state, ID, start-time, total runtime, current runtime}
	struct Task task0 = {idle, 0, 00, 30, 0};
	struct Task task1 = {idle, 1, 15, 60, 0};
	struct Task task2 = {idle, 2, 35, 40, 0};
	struct Task task3 = {idle, 3, 55, 50, 0};
	struct Task task4 = {idle, 4, 75, 20, 0};
	
	struct Task * tasks[] = {&task0, &task1, &task2, &task3, &task4};
	
	for(int i = 0; i < taskCount; i++){
		pthread_create(&threads[i], NULL, task_handler, (void*) tasks[i]);
	}
	
	sleep(1); // Let everything stabilize
	int schedulerTimeout = 250;
	
    round_robin(tasks, taskCount, schedulerTimeout, 10);

	
	for(int i = 0; i < taskCount; i++){
		pthread_join(threads[i], NULL);
	}
	
	printf("Main done \n");
	fflush(stdout);
	return 0;
}