#ifndef REF_COUNT_H
#define REF_COUNT_H

#include <pthread.h>
#include <stdint.h>

typedef struct ref_t {
	pthread_mutex_t lock;
	int ref_count;
} ref_t;

void *ref_malloc(size_t size);
void *ref_hold(void *ptr);
void ref_release(void *ptr);

#endif
