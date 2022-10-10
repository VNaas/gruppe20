#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <stdint.h>
#include <semaphore.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 5

pthread_mutex_t forks[NUM_THREADS];
pthread_barrier_t barr;


void busy_wait_ms(int ms){
	uint64_t pval = 0;
	asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(pval) :: "memory");
	uint64_t val = pval;
	while(val < pval + ms * 54054){ // With no load the RPI actually runs at 54 MHz. Check with sudo dmesg | grep MHz
		asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory");
	}
}

void* pickUpFork(void* philosopher){
    pthread_barrier_wait(&barr);
    printf("Philosopher %d Waiting for fork %d\n", (int)philosopher, (int)philosopher);

    pthread_mutex_lock(&forks[ (int)philosopher ]);
    busy_wait_ms(1000);
    printf("Philosopher %d Waiting for fork %d\n", (int)philosopher, ((int)philosopher+1 )%NUM_THREADS);

    pthread_mutex_lock(&forks[( (int)philosopher + 1 ) % NUM_THREADS]);
    return NULL;
}

int main(){
    pthread_t philosophers[NUM_THREADS];

    pthread_barrier_init(&barr,NULL, NUM_THREADS);

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_mutex_init(&forks[i], NULL);

    } // init fork mutexes

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_create(&philosophers[i],NULL,pickUpFork,i);
    }   // Create philosopher threads

    for(int i = 0; i < NUM_THREADS; i++){
        printf("%d\n",i);
        pthread_join(philosophers[i],NULL);
    }   // Join threads
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_mutex_destroy(&forks[i]);
    } // destroy fork mutexes
    return 0;
}