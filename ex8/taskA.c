#define _GNU_SOURCE
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>

#define PININ1 8
#define PINOUT1 9
#define PININ2 7
#define PINOUT2 15
#define PININ3 16
#define PINOUT3 1


// clang -s taskA.c -o taskA -std=gnu11 -g -O3 -lpthread -lwiringPi -lwiringPiDev
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

void* responseThread(void* args){
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

/** TASK A
Create 3 POSIXs threads, one for each of the tests A, B and C. Each thread waits until it receives
its test signal, and then it will send its response signal back. Run a test of 1000 subtests and store
the results.
If you get valid results in many subtests, but then suddenly there is an ”overflow”, it means
that the system was not able to reply within the 65 milliseconds that the BRTT waits for replies.
You can probably run another set of tests and that will be able to complete. If this is a consistent
problem, there is a problem with your code. */

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