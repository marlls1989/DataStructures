#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "ref_count.h"

void *ref_malloc(size_t size) {
	ref_t *ret;

	if(!(ret = malloc(size+sizeof(ref_t))))
		return NULL;

	pthread_mutex_init(&(ret->lock), 0);
	ret->ref_count = 1;

	return ret+1;
}

void *ref_hold(void *ptr) {
	ref_t *ref = ptr;
	ref--;
	
	pthread_mutex_lock(&(ref->lock));
	ref->ref_count++;
	pthread_mutex_unlock(&(ref->lock));

	return ptr;
}

void ref_release(void *ptr) {
	ref_t *ref;

	if(!(ref = ptr))
		return;
	
	ref--;
	
	pthread_mutex_lock(&(ref->lock));

	ref->ref_count--;
	
	if(!(ref->ref_count)) {
		pthread_mutex_destroy(&(ref->lock));
		free(ref);
	} else
		pthread_mutex_unlock(&(ref->lock));
}
