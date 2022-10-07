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

#define num_philosophers 5

static pthread_mutex_t *chopsticks[num_philosophers];
pthread_barrier_t barr;

void *pickUpChopsticks(void *philosopher)
{
    int right = (int)philosopher;
    int left = ( (int)philosopher+1 ) % num_philosophers;
    int odd = (int)philosopher %2;
    pthread_barrier_wait(&barr);
    while (1)
    {
        if (odd)
        {
            pthread_mutex_lock(chopsticks[right]);
            pthread_mutex_lock(chopsticks[left]);
            printf("Philosopher %d picked up chopctick %d, then %d\n", right,left);
            sleep(1);
        }
        else
        {
            pthread_mutex_lock(chopsticks[left]);
            pthread_mutex_lock(chopsticks[right]);
            printf("Philosopher %d picked up chopctick %d, then %d\n", left,right);
            sleep(1);
        }
        pthread_mutex_unlock(chopsticks[left]);
        pthread_mutex_unlock(chopsticks[right]);
        sleep(1);
    }
}

int main()
{
    pthread_t *philosophers[num_philosophers];

    pthread_barrier_init(&barr, NULL, num_philosophers);

    for (int i = 0; i < num_philosophers; i++)
    {
        pthread_mutex_init(chopsticks[i], NULL);
    } // init chopstick mutexes

    for (int i = 0; i < num_philosophers; i++)
    {
        pthread_create(philosophers[i], NULL, pickUpChopsticks, i);
    } // Create philosopher threads

    for (int i = 0; i < num_philosophers; i++)
    {
        pthread_join(philosophers[i], NULL);
    } // Join threads
    for (int i = 0; i < num_philosophers; i++)
    {
        pthread_mutex_destroy(chopsticks[i]);
    } // destroy chopstick mutexes
    return 0;
}