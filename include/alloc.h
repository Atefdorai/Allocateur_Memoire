#ifndef ALLOC_H
#define ALLOC_H

#include <unistd.h>
#include <stddef.h>

#define MOT sizeof(intptr_t)            // mot machine
#define PAGE_SIZE sysconf(_SC_PAGESIZE) // page-size addressed by the MMU

//#include <sys/mman.h>


__attribute__((always_inline)) inline size_t align(size_t size){
        return (size + (MOT-1)) & ~(MOT - 1) ;
}



typedef struct bloc_meta_data{
	size_t size     ;  // contient la taille demandé par l'utilisateur.
	char   is_free  ;  // either the bloc is freed(1) or not(0) .
	struct bloc_ *next ;	
	struct bloc_ *prev ;
} meta_data ;


typedef struct bloc_ {
	meta_data m_data ;
	void *addr_      ;
} bloc ;





void *malloc(size_t size) ;

void free(void *) ;

void *realloc(void *ptr , size_t size);

void *calloc(size_t nmemb , size_t size);




#endif 
