#ifndef ALLOC_H
#define ALLOC_H

#define MOT sizeof(uintptr_t)           // mot machine
#define PAGE_SIZE sysconf(_SC_PAGESIZE) // page-size addressed by the MMU

/*
typedef struct controle_zone {
	// meta_data 
	size_t size     ;  // contient la taille demandé par l'utilisateur.
	char   is_free  ;  // either the bloc is freed(1) or not(0) .
	struct controle_zone *next ;  // point sur le début du bloc suivant 
	//struct controle_zone *prev ;  // point sur le début du bloc précédent
} controle_zone;

typedef struct bloc {
	controle_zone meta_data ;
	void *user_zone ;
} bloc ;

*/
typedef struct bloc{
	size_t size     ;  // contient la taille demandé par l'utilisateur.
	char   is_free  ;  // either the bloc is freed(1) or not(0) .
	struct bloc *next ;	
//	struct bloc *prev ;
}


__attribute__((always_inline)) inline void align(size_t size){
        return (size + (MOT-1)) & ~(MOT - 1) ;
}











void *malloc(size_t size) ;

void free(void *) ;

void *realloc(void *ptr , size_t size);

void *calloc(size_t nmemb , size_t size);




#endif 
