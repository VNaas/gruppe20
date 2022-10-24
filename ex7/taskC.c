#define _GNU_SOURCE
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <sched.h>

#define PININ1 8
#define PINOUT1 9
#define PININ2 7
#define PINOUT2 15
#define PININ3 16
#define PINOUT3 1

// clang -s taskC.c -o taskC -std=gnu11 -g -O3 -lpthread -lwiringPi -lwiringPiDev
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

void *responseThread(void *args)
{
    int heyo = set_cpu(1);
    struct responseTaskArgs a = *(struct responseTaskArgs *)args;
    while (1)
    {
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

/** TASK C
2.2 Adding disturbances
In addition to forcing all threads to run on the same CPU-core, we will also create additional
threads that run on the same core just to disturb the application. Each of these disturbance threads
should set themselves to run on the same core, before starting an infinite loop of either some simple
calculations, or just
while(1){
asm volatile("" ::: "memory");
}
TASK C
Spawn 10 disturbance threads running an infinite loop, on the same CPU core as the response task
threads. Record those results - again, an A+B+C test with 1000 subtests. Are you able to get
all 1000 subtests to run? How many disturbance threads can the system handle without getting
overflow on the BRTT? Hint: it should be less than 10.
We will get back to comparing these results in task D.
*/

int main()
{
    io_init();
    pthread_t threadA;
    pthread_t threadB;
    pthread_t threadC;
    int num_disturbances = 9;

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