#include "../include/alloc.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
pthread_mutex_t m_mut = PTHREAD_MUTEX_INITIALIZER;

void * malloc(size_t size){

	void *bloc_to_ret = NULL ;
        bloc	*addr = NULL;
	bloc *addr_meta = NULL;

	if (size == 0)
		return NULL;
	pthread_mutex_lock(&m_mut);

	size_t true_size = align (sizeof(bloc) + size) ;	 
	
	static bloc *bloc_first_alloc = NULL ; // change in first allocation  
	static bloc *prev_alloc       = NULL ; // change in every allocation 
	
	//if( size % )


	if(bloc_first_alloc != NULL){
		/* check for a free bloc */
		bloc *bloc_iter = bloc_first_alloc ;
		while(bloc_iter -> m_data.is_free == 1){
			if (bloc_iter -> m_data.size >= true_size){
				bloc *tmp;
				tmp = bloc_iter;
				tmp -> m_data.size    = true_size;
				tmp -> m_data.is_free = 0 ;
				tmp -> m_data.next    = tmp + true_size;
				tmp -> m_data.prev    = prev_alloc ;
				tmp -> addr_ = tmp + sizeof(meta_data);
				prev_alloc = tmp + sizeof(meta_data) ;
				pthread_mutex_unlock(&m_mut);
				return tmp -> addr_; // return user address
			}
			bloc_iter  = bloc_iter -> m_data.next ;
		}
		/* bloc not found in the free list */
		addr = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1 , 0);
		if (addr == MAP_FAILED){
			perror("not found in the free list");
			pthread_mutex_unlock(&m_mut);
			return NULL;
		}
		addr_meta = addr ;
		addr_meta -> m_data.size    = true_size;
		addr_meta -> m_data.is_free = 0;
		addr_meta -> m_data.next    = addr_meta + true_size;
		addr_meta -> m_data.prev    = prev_alloc ;
		addr_meta -> addr_          = addr_meta + sizeof(meta_data);
		prev_alloc = addr_meta  ;
		pthread_mutex_unlock(&m_mut);
		return addr_meta -> addr_ ;  // return user address
	}
	else {
		/* first allocation */
		addr = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1 , 0);
		if (addr == MAP_FAILED){
			perror("not found in the free list");
			return NULL;
		}
		addr_meta = addr;
		addr_meta -> m_data.size    = true_size;
		addr_meta -> m_data.is_free = 0;
		addr_meta -> m_data.next    = addr_meta + true_size;
		addr_meta -> m_data.prev    = NULL ; // first allocation
		addr_meta -> addr_          = addr_meta + sizeof(meta_data);
		bloc_first_alloc = addr_meta + sizeof(meta_data); // adresse de première allocation
		prev_alloc = bloc_first_alloc ;
		pthread_mutex_unlock(&m_mut);
		return addr_meta -> addr_ ; //return user address
	}
}


void free(void* ptr){
	//bloc *this_bloc = ptr ;
	pthread_mutex_lock(&m_mut);

	bloc *this_free_bloc  = (bloc *)(ptr - sizeof(meta_data)) ;
	this_free_bloc -> m_data.is_free = 1 ; // meta_data.size unchanged immediately !
	
	/* coleseaing free blocs */
	bloc *check_next_bloc = this_free_bloc -> m_data.next,
	     *check_prev_bloc = this_free_bloc -> m_data.prev;
		
	while(check_next_bloc -> m_data.is_free == 1){
		//bloc *tmp = check_next_bloc ;
		this_free_bloc -> m_data.size += check_next_bloc -> m_data.size ;
		this_free_bloc -> m_data.next  = check_next_bloc -> m_data.next ;
		//ptr = this_free_bloc ;

		if(check_next_bloc -> m_data.next == NULL)
			break ;
		check_next_bloc = check_next_bloc -> m_data.next ;		
	}
	while(check_prev_bloc -> m_data.is_free == 1){
		//bloc *tmp = check_prev_bloc ;
		size_t tmp_size = check_prev_bloc -> m_data.size;
		bloc *tmp_free = this_free_bloc ;
		
		this_free_bloc = check_prev_bloc ; 
		this_free_bloc -> m_data.size += tmp_size ;
		this_free_bloc -> m_data.next  = tmp_free;
		
		ptr = (void *)this_free_bloc ;
		if(check_prev_bloc -> m_data.prev == NULL)
			break ;
		check_prev_bloc = check_prev_bloc -> m_data.prev ;		
	}
	pthread_mutex_unlock(&m_mut);

}



void * calloc(size_t nmemb, size_t size){
	if(nmemb == 0 || size == 0)
		return NULL;
	if (nmemb != (nmemb * size) / size)
		return NULL;
	return malloc(nmemb * size);	
}


void * realloc(void* ptr, size_t size){
	if (ptr == NULL)
		return malloc(size);
	else if( (size == 0) && (ptr == NULL) ){
		free(ptr);
		return NULL;
	}
	else{
		pthread_mutex_lock(&m_mut);
		bloc *tmp2  = ((bloc *)(ptr) - sizeof(meta_data)) -> m_data.next,
		     *tmp   = (bloc *)(ptr) - sizeof(meta_data) ; 

		size_t true_size = align(size + sizeof(meta_data));
		size_t res_size = true_size - tmp -> m_data.size ;
		bloc *tmp1 = tmp2 -> m_data.next ;
		if ( (tmp2 -> m_data.is_free == 1) && (tmp2 -> m_data.size > res_size ) ){
 			size_t tmp_size = tmp2 -> m_data.size ;
			tmp2 += res_size ; 
			tmp2 -> m_data.size   -= res_size ;
			tmp2 -> m_data.is_free = 1 ;
			tmp2 -> m_data.next    = tmp1 ;
			tmp2 -> m_data.prev    = tmp ;

			tmp -> m_data.size = true_size ;
			tmp -> m_data.next = tmp2 ;
			tmp -> m_data.is_free       = 0 ;
			pthread_mutex_unlock(&m_mut);
			return tmp;
		}		
		
		void *ptr_ = malloc(size);
		if (ptr_ != NULL){
			memcpy(ptr_ , ptr, tmp -> m_data.size);
			free(ptr);
			pthread_mutex_unlock(&m_mut);
			return ptr_;
		}

		perror("can't realloc");
		return NULL;
	}
}
