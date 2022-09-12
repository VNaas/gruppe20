#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "array.h"


// Construction / Destruction

Array array_new(long capacity){
    assert(capacity > 0);
    return (Array){malloc(sizeof(long)*capacity), 0, 0, capacity};
}

void array_destroy(Array a){
    free(a.data);
}


// Primitives

long array_empty(Array a){
    return a.back <= a.front;
}

long array_front(Array a){
    return a.data[a.front];
}

long array_back(Array a){
    return a.data[a.back - 1];
}

void array_popFront(Array* a){
    a->front++;
}

void array_popBack(Array* a){
    a->back--;
}

Array array_save(Array a){
    return (Array){a.data, a.front, a.back, a.capacity};
}


// Iteration

void array_foreach(Array a, void fn(long)){
    for(Array b = array_save(a); !array_empty(b); array_popFront(&b)){
        fn(array_front(b));
    }
}

void array_foreachReverse(Array a, void fn(long)){
    for(Array b = array_save(a); !array_empty(b); array_popBack(&b)){
        fn(array_back(b));
    }
}

static void _array_printSingleLongHelper(long i){
    printf(", %ld", i);
}

void array_print(Array a){
    printf("Array:{");
    if(!array_empty(a)){
        printf("%ld", array_front(a));
        array_popFront(&a);
    }
    array_foreach(a, _array_printSingleLongHelper);
    printf("}\n");
}


// Capacity

long array_length(Array a){
    return a.back - a.front;
}

void array_reserve(Array* a, long capacity){
    long multiplier = 2;
    long* temp = malloc(sizeof(long)*capacity*multiplier);
    memcpy(temp,a -> data,(a->capacity)*sizeof(long));
    free(a->data);
    a -> data = temp;
    a -> capacity = capacity*multiplier;
}


// Modifiers

void array_insertBack(Array* a, long stuff){
    if(a->capacity == a -> back){
        array_reserve(a,a->capacity);
    }
    printf("Back: %li\n", a -> back);
    printf("Cap: %li\n", a -> capacity);

    a -> data[a -> back] = stuff;
    a -> back++;
}

int main(){
    // TASK A
    // long xy_size = 1000*1000*500;
    // long x_dim = 10000;
    // long y_dim = xy_size/x_dim;
    // long** matrix = malloc(y_dim*sizeof(long*));

    // for(long y = 0; y < y_dim; y++){
    //     memset(matrix[y],0,x_dim*sizeof(long));
    //     matrix[y] = malloc(x_dim*sizeof(long));
    //     // memset => seg fault. malloc not allocate right away
    // }
    // // Allocating using xdim=100 is way way slower. Maybe because malloc(ydim...)
    // // Needs to find much larger chunks of consecutiv memory, and has to use more
    // // swap space, as these arrays cant fit into main memory.
    // // However,how does this fit with our observation of resource use?
    // // xdim = 100 took u about 3000 MB of memory, while xdim = 10000 took rougly 200MB.
    // printf("Allocation complete (press any key to continue .. )\n");
    // getchar();

    // TASK B

    Array array = array_new(2);
    long a = 1;
    long b = 2;
    long c = 3;
    array_insertBack(&array,a);
    array_print(array);
    // printf("Back: %li\n", array_back(array));
    array_insertBack(&array,b);
    array_print(array);
    // printf("Back: %li\n", array_back(array));
    // printf("Everything's good\n");
    array_insertBack(&array,c);
    array_print(array);
    // printf("Back: %li\n", array_back(array));
    // printf("Cap %li \n", array.capacity);
    array_destroy(array);
}
