#include <alloc.h>
#include <random.h>
#include <unistd.h>
#include <time.h>

#define RUNS 100000
#define MAX  1000

int main(int argc, char **argv){
	srand(0);
	double *ptr[RUNS];
	for(int i = 0 ; i < RUNS ; ++i){
		size_t size = rand() % MAX;
		ptr[i] = malloc(size * sizeof(double));
	}
	for(int i = 0 ; i < RUNS ; ++i){
		
	}

	return 0 ;
}
