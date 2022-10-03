//clang -s raceCondition.c -o raceCondition -std=gnu11 -g -O3 -lpthread

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

volatile long global_int = 0;
pthread_barrier_t barr;
sem_t mutex;

void* increment(void* args){
    long local_int = 0;
    for (int i = 0; i <1000000;i++){
        local_int ++;
        sem_wait(&mutex);
        global_int ++;
        sem_post(&mutex);
    }
    int res = pthread_barrier_wait(&barr);
    printf("My result: local: %ld\tglobal:%ld \n",local_int,global_int);
    return NULL;
}

int main(){
    pthread_t threadHandle1;
    pthread_t threadHandle2;
    int n_threads = 2;
    sem_init(&mutex,0,1);
    pthread_barrier_init(&barr,NULL,n_threads);
    pthread_create(&threadHandle1, NULL, increment,NULL);
    pthread_create(&threadHandle2, NULL, increment,NULL);
    pthread_join(threadHandle1,NULL);
    pthread_join(threadHandle2,NULL);
    sem_destroy(&mutex);
}

/* BEFORE SYNC
time ./raceCondition
My result: local: 1000000	global:1002209 
My result: local: 1000000	global:1623468 

real	0m0.053s
user	0m0.021s
sys	    0m0.042s
*/

/* AFTER SYNC
My result: local: 1000000	global:2000000 
My result: local: 1000000	global:2000000 

real	0m3.972s
user	0m3.979s
sys	    0m3.868s
 */