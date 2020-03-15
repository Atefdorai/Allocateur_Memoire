#include "./include/alloc.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define RUNS 1000
#define MAX  1000

int main(int argc, char **argv){
	srand(0);
	
	double *ptr[RUNS];

	//ptr[0] = malloc(sizeof(double) * 40000);	
	//printf("malloc(%d) : %p\n" , 40000, ptr[0]);
	
	clock_t start, end ;
	double cpu_time_used ;

	start = clock() ;
	for(int i = 0 ; i < RUNS ; ++i){
		size_t size = rand() % MAX;
		ptr[i] = malloc(size * sizeof(double));
		//printf("malloc(%ld) : %p\n" , size, ptr[i]);

	}
        
	for(int i = 0 ; i < RUNS / 2 ; ++i){

		free(ptr[(i + rand()) % RUNS]);		
	}



	for(int i = 0 ; i < 10 ; ++i){
		size_t size = rand() % MAX;
		ptr[i] = malloc(size * sizeof(double));
		//printf("malloc(%ld) : %p\n" , size, ptr[i]);

	}

	end = clock() ;
	cpu_time_used = ((double)(end - start)) / CLOCK_PER_SC ;  

	printf("temps écoulé : %f\n" cpu_time_used);

	return 0 ;
}
