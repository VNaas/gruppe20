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

// clang -s taskB.c -o taskB -std=gnu11 -g -O3 -lpthread -lwiringPi -lwiringPiDev
void io_init( void ) {
    wiringPiSetup();
    pinMode (PININ1, INPUT); pinMode (PINOUT1, OUTPUT);
    pinMode (PININ2, INPUT); pinMode (PINOUT2, OUTPUT);
    pinMode (PININ3, INPUT); pinMode (PINOUT3, OUTPUT);
    digitalWrite(PINOUT1, HIGH);
    digitalWrite(PINOUT2, HIGH);
    digitalWrite(PINOUT3, HIGH);
}

struct responseTaskArgs {
    struct { 
        uint32_t test; 
        uint32_t response;
    } pin;
	uint32_t delayDuration;
};

int set_cpu(int cpu_number){
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* responseThread(void* args){
    int heyo = set_cpu(1);
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
    while(1){
		if(!digitalRead(a.pin.test)){
			digitalWrite(a.pin.response,LOW);
			int pin_value = 0;
			while(!pin_value){
				pin_value = digitalRead(a.pin.test);
			}
			digitalWrite(a.pin.response,HIGH);
		}
	}
}

/** TASK B
First of all we want to run everything on the same CPU core. The following function can be called
in the start of a spawned thread function, and will ensure that the thread only will be executed on
the specified CPU core. 

Run the same test as in A, with all threads on the same CPU core, and record the results for an
A+B+C test with 1000 iterations.
Compare the results between running on a single core and all four. Compare the shape of the
distributions: do any of the histograms have any particular spikes, and why?
Which one has the best worst-case response time, and why?
*/

int main(){
    io_init();
    pthread_t threadA;
    pthread_t threadB;
    pthread_t threadC;



    pthread_create(&threadA, NULL, responseThread, (&(struct responseTaskArgs){{PININ1, PINOUT1},0}));
    pthread_create(&threadB, NULL, responseThread, (&(struct responseTaskArgs){{PININ2, PINOUT2},0}));
    pthread_create(&threadC, NULL, responseThread, (&(struct responseTaskArgs){{PININ3, PINOUT3},0}));

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);

    return 0;
}