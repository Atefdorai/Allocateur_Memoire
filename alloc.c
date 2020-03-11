#include <alloc.h>
#include <sys/mman.h>
#include <unistd.h>

//bloc *g_bloc = NULL;
//bloc *this_bloc ;

static bloc *first_alloc ;

void * malloc(size_t size){
	void *ret = NULL , *ptr = NULL;

	if (size == NULL)
		return NULL;

	size_t true_size = align (sizeof(bloc) + align(size)) ;	
	
	//static actual_available_size = 0 ; // global! 
	
	static bloc *this_bloc = NULL ; // = first_ptr 

	if(this_bloc != NULL){ // first allocation
		/* check for a free bloc */
		while(this_bloc->size != 0){
			if (this_bloc->is_free == 1) && (this_bloc->size >= true_size){
				void *tmp;
				tmp = this_bloc;
				tmp->size = true_size;
				tmp->is_free = 0 ;
				tmp->next = tmp + true_size;
				//tmp->prev =
				return tmp /* + sizeo(meta_data)*/  ;
			}
			this_bloc  = this_bloc -> next;
		}
		/* bloc not found in the implicit free list */
		ret = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1 , 0);
		ret->size = true_size;
		ret->is_free = 0;
		ret->next = ret + true_size ;
		return ret /* + sizeof(meta_data)*/;
	
	}
	else {
		/* first allocation */
		ret = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1 , 0);
		ret->size = true_size;
		ret->is_free = 0;
		ret->next = ret + true_size ;
		first_alloc = ret;
		return ret /* + meta_data*/
	
	}
}

void * calloc(size_t nmemb, siez_t size){
	if(nmemb == 0 || size == 0)
		return NULL;
	if (nmemb != (nmemb * size) / size)
		return NULL;
	return malloc(nmemb * size);	
}


void * realloc(void* ptr, size_t size){
	if (ptr == NULL)
		return malloc(size);
	else if (size == 0) && (ptr == NULL){
		free(ptr);
		return NULL;
	}
	else{
		bloc *tmp = ptr, *tmp2; 
		if (tmp->next->is_free == 1) && (tmp->next->size > size){
			tmp  = tmp->next ;
			size_t tmp_size = tmp->size ;
			tmp2 = tmp->next ;
			tmp->size = size ;
			tmp->is_free = 0 ;
			tmp->next = tmp2 - tmp_size;
			return tmp;
		}		
		tmp = malloc(size);
		if(tmp != NULL){
			free(ptr);
			return tmp;
		}
	}
}

void free(void* ptr){
	//bloc *this_bloc = ptr ;
	if (ptr->next -> is_free == 1){
		size_t tmp_size = ptr->next->size + sizeof(bloc);
		bloc *tmp = ptr->next -> next ;
		ptr->size += tmp_size ;
		ptr->next = tmp ;
		ptr->is_free = 1;
	}
	if(ptr->prev -> is_free == 1){
		tmp_size = ptr->size + sizeof(bloc) + ptr->prev->size;
		ptr = ptr->prev ;
		ptr->size += tmp_size ;
		ptr->is_free = 1;

	}

}


