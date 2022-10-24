#define _GNU_SOURCE
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <sched.h>
#include "time.h"

#define PININ1 8
#define PINOUT1 9
#define PININ2 7
#define PINOUT2 15
#define PININ3 16
#define PINOUT3 1

// clang -s taskD.c -o taskD -std=gnu11 -g -O3 -lpthread -lwiringPi -lwiringPiDev
void io_init(void)
{
    wiringPiSetup();
    pinMode(PININ1, INPUT);
    pinMode(PINOUT1, OUTPUT);
    pinMode(PININ2, INPUT);
    pinMode(PINOUT2, OUTPUT);
    pinMode(PININ3, INPUT);
    pinMode(PINOUT3, OUTPUT);
    digitalWrite(PINOUT1, HIGH);
    digitalWrite(PINOUT2, HIGH);
    digitalWrite(PINOUT3, HIGH);
}

struct responseTaskArgs
{
    struct
    {
        uint32_t test;
        uint32_t response;
    } pin;
    uint32_t delayDuration;
};

int set_cpu(int cpu_number)
{
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void timespec_add(struct timespec *t, long us)
{
    t->tv_nsec += us * 1000;
    if (t->tv_nsec > 1000000000)
    {
        t->tv_nsec = t->tv_nsec - 1000000000;
        t->tv_sec += 1;
    }
}

void *responseThread(void *args)
{
    int heyo = set_cpu(1);
    struct responseTaskArgs a = *(struct responseTaskArgs *)args;
    long period = 2000; // in us
    struct timespec waketime;
    while (1)
    {
        // do periodic work ...
        if (!digitalRead(a.pin.test))
        {
            digitalWrite(a.pin.response, LOW);
            int pin_value = 0;
            while (!pin_value)
            {
                pin_value = digitalRead(a.pin.test);
            }
            digitalWrite(a.pin.response, HIGH);
        }
        // sleep
        clock_gettime(CLOCK_REALTIME, &waketime);
        timespec_add(&waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
}

void *annoyingThread(void *args)
{
    int heyo = set_cpu(1);
    while (1)
    {
        asm volatile("" ::
                         : "memory");
    }
}

/** TASK D
Create three tasks that periodically poll the test signal from the BRTT, and set its response signal
as soon it receives it. The period should be 2ms, but feel free to experiment with shorter periods.
What happens if the period is too short?

As before, you should run all threads on the same CPU. Do an A+B+C reaction test with 1000
subtests, with and without disturbance. Compare the four results: busy-polling the input vs periodic
execution, both with and without disturbance:

Without the disturbance enabled - which one has the fastest response, and why?
Which one is less impacted by the addition of the disturbance, and why?
When the disturbance is enabled - do any of these have a predictable and consistent worst-case
response time?
*/

int main()
{
    io_init();
    pthread_t threadA;
    pthread_t threadB;
    pthread_t threadC;
    int num_disturbances = 8;

    pthread_t annoyingThreads[num_disturbances];
    for (int i = 0; i < num_disturbances; i++)
    {
        pthread_create(&annoyingThreads[i], NULL, annoyingThread, NULL);
    }

    pthread_create(&threadA, NULL, responseThread, (&(struct responseTaskArgs){{PININ1, PINOUT1}, 0}));
    pthread_create(&threadB, NULL, responseThread, (&(struct responseTaskArgs){{PININ2, PINOUT2}, 0}));
    pthread_create(&threadC, NULL, responseThread, (&(struct responseTaskArgs){{PININ3, PINOUT3}, 0}));

    for (int i = 0; i < num_disturbances; i++)
    {
        pthread_join(annoyingThreads[i], NULL);
    }

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);

    return 0;
}