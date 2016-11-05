#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <pthread.h>

typedef struct queue_t {
	pthread_mutex_t lock;
	pthread_cond_t wait;
	volatile size_t reads, writes;
	size_t size;
	void **data;
} queue_t;

void enqueue(void *element, queue_t *queue);
void *dequeue(queue_t *queue);

void init_queue(queue_t *queue);
void destroy_queue(queue_t *queue);

#endif
