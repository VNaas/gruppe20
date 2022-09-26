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
	int priority;
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
	printf("Task with ID %d finishing after having worked for %d out of %d time units \n", task->ID, task->currentRuntime, task->totalRuntime);
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
void first_come_first(struct Task ** tasks, int taskCount, int timeout){
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
		
		for(int i = 0; i < taskCount; i++){
			if(tasks[i] ->startTime < currentTaskStartTime || currentTaskStartTime == -1){
				if (tasks[i] -> state != finished){
				currentTaskIndex = i;
				currentTaskStartTime = tasks[i] -> startTime;
				}
			}
		}
		// printf("TASK INDEX: %i \n", currentTaskIndex);
		if(tasksCompleted < taskCount){
			set_task_state(tasks[currentTaskIndex], running);
			wait_for_rescheduling(tasks[currentTaskIndex] -> totalRuntime+10, tasks[currentTaskIndex]);
			set_task_state(tasks[currentTaskIndex], finished);
			currentTaskStartTime = -1;
			tasksCompleted++;
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime);
		}
		if(tasksCompleted >= taskCount){
			totalTimePassed++;
		}
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			if(tasksCompleted == taskCount){
				printf("All tasks completed\n");
			}
			fflush(stdout);
			return;
		}
	}
}

void shortest_process_next(struct Task ** tasks, int taskCount, int timeout){
	printf("Started SPN scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	// int currentTaskIndex = 0;
	// int prevTaskIndex = -1;
	int tasksCompleted = 0;
	int currentTaskRunTime = -1;
	int currentTaskIndex = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		for(int i = 0; i < taskCount; i++){
			if(tasks[i] ->totalRuntime < currentTaskRunTime || currentTaskRunTime == -1){
				if (tasks[i] -> state != finished){
				currentTaskIndex = i;
				currentTaskRunTime = tasks[i] -> totalRuntime;
				}
			}
		}
		// printf("TASK INDEX: %i \n", currentTaskIndex);
		if(tasksCompleted < taskCount){
			set_task_state(tasks[currentTaskIndex], running);
			wait_for_rescheduling(tasks[currentTaskIndex] -> totalRuntime+10, tasks[currentTaskIndex]);
			set_task_state(tasks[currentTaskIndex], finished);
			currentTaskRunTime = -1;
			tasksCompleted++;
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime);
		}
		if(tasksCompleted >= taskCount){
			totalTimePassed++;
		}
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			if(tasksCompleted == taskCount){
				printf("All tasks completed\n");
			}
			fflush(stdout);
			return;
		}
	}
}

double calculate_response_ratio(struct Task* task, int totalTimePassed){
	double s = (task->totalRuntime - task->currentRuntime);
	double w = totalTimePassed - task -> startTime;
	return (w + s)/s;
}

void highest_RR_next(struct Task ** tasks, int taskCount, int timeout){
	printf("Started HRRN scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	// int currentTaskIndex = 0;
	// int prevTaskIndex = -1;
	int tasksCompleted = 0;
	int currentResponseRatio = -100000;
	int currentTaskIndex = 0;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		
		if(tasksCompleted < taskCount){
			for(int i = 0; i < taskCount; i++){
				if (tasks[i] -> state != finished){
					double taskResponseRatio = calculate_response_ratio(tasks[i], totalTimePassed);
					printf("RR: %f for index:%d \n", taskResponseRatio, i);
					if(taskResponseRatio > currentResponseRatio || currentResponseRatio == -100000){
						currentTaskIndex = i;
						currentResponseRatio = taskResponseRatio;
						
					}
				}
			}
			set_task_state(tasks[currentTaskIndex], running);
			wait_for_rescheduling(tasks[currentTaskIndex] -> totalRuntime+10, tasks[currentTaskIndex]);
			set_task_state(tasks[currentTaskIndex], finished);
			currentResponseRatio = -100000;
			tasksCompleted++;
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime);
		}
		if(tasksCompleted >= taskCount){
			totalTimePassed++;
		}
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			if(tasksCompleted == taskCount){
				printf("All tasks completed\n");
			}
			fflush(stdout);
			return;
		}
	}
}

int next_start_time(struct Task** tasks, int taskCount){
	int nextStartTime = -1;
	for(int i = 0; i < (taskCount-1); i++){
		// if((nextStartTime == -1 && tasks[i] == idle)|| (tasks[i]->startTime < nextStartTime && tasks[i]-> state == idle)){
		// 	nextStartTime = tasks[i] -> startTime;
		// }
		if((nextStartTime == -1) && (tasks[i]->state == idle)){
			nextStartTime = tasks[i] -> startTime;
		}
		else if (tasks[i]->startTime < nextStartTime && tasks[i]->state == idle ){
			nextStartTime = tasks[i] -> startTime;
		}		
	}
	printf("Next starttime: %i \n",nextStartTime);

	return nextStartTime;
}

int calculate_interval(struct Task** tasks, int taskCount, int currentTaskIndex, int totalTimePassed, int currentTaskRemainingTime){
	int interval = currentTaskRemainingTime;
	for(int i = 0; i < taskCount; i++){
		if(tasks[i] ->state != finished){
			if(tasks[i] -> startTime > totalTimePassed){
				if((tasks[i]->startTime-totalTimePassed) < interval)
				interval = tasks[i]->startTime-totalTimePassed;
			}
		}
	}
	return interval;
}

void shortest_remaining_time(struct Task ** tasks, int taskCount, int timeout){
	printf("Started SRT scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int tasksCompleted = 0;
	int currentTaskRemainingTime = -1;
	int currentTaskIndex = 0;
	// int prevTaskIndex = -1;
	double rt = 5;
	int interval = rt;
	while(1){
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);
		currentTaskRemainingTime = -1;
		if(tasksCompleted < taskCount){

			for(int i = 0; i < taskCount; i++){
				if (tasks[i]-> state != finished){
					if(tasks[i] ->startTime <= totalTimePassed){
						rt = tasks[i]-> totalRuntime - tasks[i]-> currentRuntime;
						if (rt < currentTaskRemainingTime || currentTaskRemainingTime == -1){
							currentTaskIndex = i;
							currentTaskRemainingTime = rt;
							printf("Current task: %i  with rt: %f\n",currentTaskIndex, rt);
						}
					}
				}
			}
			// CALCULATE INTERVAL
			interval = calculate_interval(tasks, taskCount, currentTaskIndex, totalTimePassed, currentTaskRemainingTime);
			printf("First interval: %d \n", interval);
			set_task_state(tasks[currentTaskIndex], running);
			wait_for_rescheduling(interval,tasks[currentTaskIndex]);
			// DID IT FINISH
			if(tasks[currentTaskIndex]->currentRuntime == tasks[currentTaskIndex]->totalRuntime){
				set_task_state(tasks[currentTaskIndex],finished);
				tasksCompleted++;
			}
			else
			{
				set_task_state(tasks[currentTaskIndex],preempted);
			}
			//DO AFTER RESCHEDULE
			totalTimePassed += (interval);
		}
		if(tasksCompleted >= taskCount){
			totalTimePassed++;
		}
		if(totalTimePassed >= timeout){
			printf("Scheduler done \n");
			if(tasksCompleted == taskCount){
				printf("All tasks completed\n");
			}
			fflush(stdout);
			return;
		}
	}
}

void feedback(struct Task ** tasks, int taskCount, int timeout){
	while(1){
	printf("Started RR scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;
	int interval = 5;
	int currentPriority = 0;	//Do something with current priority?
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


int main(){
	printf("Main started \n");
	fflush(stdout);
	int taskCount = 5;
	pthread_t threads[taskCount];
	
	// {state, ID, start-time, total runtime, current runtime}
	struct Task task0 = {idle, 0, 00, 30, 0, 0};
	struct Task task1 = {idle, 1, 15, 60, 0, 0};
	struct Task task2 = {idle, 2, 35, 40, 0, 0};
	struct Task task3 = {idle, 3, 55, 50, 0, 0};
	struct Task task4 = {idle, 4, 75, 20, 0, 0};
	
	struct Task * tasks[] = {&task0, &task1, &task2, &task3, &task4};
	
	for(int i = 0; i < taskCount; i++){
		pthread_create(&threads[i], NULL, task_handler, (void*) tasks[i]);
	}
	
	sleep(1); // Let everything stabilize
	int schedulerTimeout = 250;
	
    // round_robin(tasks, taskCount, schedulerTimeout, 10);
    //TASK B:
	//first_come_first(tasks, taskCount, schedulerTimeout);
	//shortest_process_next(tasks, taskCount, schedulerTimeout);
	//highest_RR_next(tasks, taskCount,schedulerTimeout);
	
	//TASK C:
	// shortest_remaining_time(tasks, taskCount,schedulerTimeout);
	feedback(tasks, taskCount,schedulerTimeout);


	
	for(int i = 0; i < taskCount; i++){
		pthread_join(threads[i], NULL);
	}
	
	printf("Main done \n");
	fflush(stdout);
	return 0;
}