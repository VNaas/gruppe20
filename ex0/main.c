#include <stdio.h>
#include "sort.h"

int main(int argc, char* argv[]){
    int i;
    printf("Arguments before sorting:\n");
    for(i = 1; i < argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");
    sort(argc,argv);
    printf("Arguments after: \n");

    for(i = 1; i < argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}