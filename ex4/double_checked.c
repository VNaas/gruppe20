//clang -s double_checked.c -o double_checked -std=gnu11 -g -O3 -lpthread
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

static inline void nonOptimizedBusyWait(void){
    for(long i = 0; i < 10000000; i++){
        // "Memory clobber" - tells the compiler optimizer that all the memory
        // is being touched, and that therefore the loop cannot be optimized out
        asm volatile("" ::: "memory");
    }
}


struct Singleton {
    // Creating some dummy members
    long a;
    long b;
    long c;
    long d;
};


static struct Singleton*    g_singleton         = NULL;
sem_t                       g_singletonInitSem;



struct Singleton* getSingleton(){
    if(!g_singleton){
        sem_wait(&g_singletonInitSem);
        if(!g_singleton){
            // Performing some expensive initialization...
            printf("Initializing singleton...\n");
            nonOptimizedBusyWait();
            g_singleton = malloc(sizeof(struct Singleton));
            nonOptimizedBusyWait();
            g_singleton->a = 1;
            nonOptimizedBusyWait();
            g_singleton->b = 22;
            nonOptimizedBusyWait();
            g_singleton->c = 333;
            nonOptimizedBusyWait();
            g_singleton->d = 4444;
            nonOptimizedBusyWait();
        }
        sem_post(&g_singletonInitSem);
    }
    return g_singleton;
}
/* The initialization of the Singelton g_singelton object in line 38 is done in three steps:
1: allocate memory
2: construct Singelton object in allocated memory
3: make g_simpelton point to the object.
C/C++ DOES NOT GUARANTEE that step 2 and three happen in that order.
This makes the action not reliable:
    Thread A -> step 1, step 3;
    Thread B -> check g_singleton - not NULL; return g_singleton
Here Thread B returns the pointer before the object is created.
*/


void* singletonUser(void* args){

    nonOptimizedBusyWait();

    struct Singleton* s = getSingleton();

    printf("Got singleton %p:{%lu %lu %lu %lu}\n",
        (void*)s, s->a, s->b, s->c, s->d);

    return NULL;
}


int main(){
    sem_init(&g_singletonInitSem, 0, 1);

    pthread_t threadHandles[50];
    long numThreads = sizeof(threadHandles)/sizeof(*threadHandles);

    for(long i = 0; i < numThreads; i++){
        pthread_create(&threadHandles[i], NULL, singletonUser, NULL);
    }

    for(long i = 0; i < numThreads; i++){
        pthread_join(threadHandles[i], NULL);
    }
}