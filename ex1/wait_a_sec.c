#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <string.h>
#include <inttypes.h>
struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
    nsec -= 1000000000;
    ++sec;
}
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
        }
    return (struct timespec){sec, nsec};
}
struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}
struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}
int timespec_cmp(struct timespec lhs, struct timespec rhs){
    if (lhs.tv_sec < rhs.tv_sec){
        return -1;
    }
    if (lhs.tv_sec > rhs.tv_sec){
        return 1;
    }
    return lhs.tv_nsec - rhs.tv_nsec;
}

void busy_wait(struct timespec t){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC,&now);
    struct timespec then = timespec_add(now,t);

    while(timespec_cmp(now,then)<0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void busy_wait_times(clock_t t){
    static struct tms st_cpu;
    static struct tms en_cpu;
    static clock_t st_time;
    static clock_t en_time;
    st_time = times(&st_cpu);
    while (((en_time-st_time))<t*100){
        en_time = times(&en_cpu);
        // printf("T: %ld",CLOCKS_PER_SEC);
        printf("Time taken = %ld ticks (%lf milliseconds)\n",(en_time - st_time), 1000.0 * (en_time - st_time)/CLOCKS_PER_SEC);   

    }

}
uint64_t rdtsc(void){
    uint64_t val;
    asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory");
    return val;
}


int main(int argc, char* argv[]){
    //struct timespec mytime;
    //mytime.tv_sec = 1;
    //mytime.tv_nsec = 0L;
    //nanosleep(&mytime,NULL);
    //busy_wait(mytime);
    //clock_t timetime = 1;
    //busy_wait_times(timetime);
    //TASK B:
    
    // uint64_t end;
    // uint64_t start = rdtsc();
    // for(int i = 0; i < 10*1000*1000; i++){
    //     end = rdtsc();
    // }
    // int64_t total_rdtsc = (end-start)/10000000;

    
    // struct timespec endTime;
    // struct timespec startTime;
    // clock_gettime(CLOCK_MONOTONIC,&startTime);
    // for(int i = 0; i < 10*1000*1000; i++){
    //     clock_gettime(CLOCK_MONOTONIC,&endTime);
    // }
    // struct timespec total_gettime = timespec_sub(endTime,startTime);
    // printf("Estimated latency: ");

    // printf("Total time: %" PRIu64,total);
    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
    for(int i = 0; i < 10*1000*1000; i++){
        //RDSC
        //uint64_t t1 = rdtsc();
        //uint64_t t2 = rdtsc();
        //int ns = (t2 - t1) *18.51 ;
        //GET-time
        //struct timespec t1, t2;
        // clock_gettime(CLOCK_MONOTONIC,&t1);
        // clock_gettime(CLOCK_MONOTONIC,&t2);
        // int ns = (t2.tv_nsec-t1.tv_nsec);
        //TIMES
        clock_t t1;
        clock_t t2;
         static struct tms st_cpu;
        t1 = times(&st_cpu);
        t2 = times(&st_cpu);
        int ns = (s)

        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }
    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }


    return 0;
}