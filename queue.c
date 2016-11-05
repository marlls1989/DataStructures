#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "queue.h"

//size should always be a power of 2
#define QUEUE_INITIAL_SIZE 16

void init_queue(queue_t *queue) {
	pthread_mutex_init(&(queue->lock), NULL);
	pthread_cond_init(&(queue->wait), NULL);

	queue->data = malloc(QUEUE_INITIAL_SIZE*sizeof(void*));
	queue->size = QUEUE_INITIAL_SIZE;
	queue->reads = queue->writes = 0;
}

void destroy_queue(queue_t *queue) {
	pthread_mutex_destroy(&(queue->lock));
	pthread_cond_destroy(&(queue->wait));

	free(queue->data);
}

//size should always be a power of 2
static void resize_queue(queue_t *queue, size_t new_size) {
	void **new_data;
	size_t i, k;

	new_data = malloc(new_size*sizeof(void*));

	for(i = 0, k = queue->writes - queue->reads ; i  < k ; i++)
		new_data[i] = queue->data[(queue->reads+i) & (queue->size-1)];

	free(queue->data);

	queue->data = new_data;
	queue->size = new_size;
	queue->writes = k;
	queue->reads = 0;
}

void enqueue(void *element, queue_t *queue) {
	size_t s;
	pthread_mutex_lock(&(queue->lock));

	if(!(s = queue->writes - queue->reads))
		queue->reads = queue->writes = 0;

	if(s >= queue->size)
		resize_queue(queue, queue->size << 1);

	queue->data[(queue->writes++) & (queue->size-1)] = element;

	pthread_cond_broadcast(&(queue->wait));
	pthread_mutex_unlock(&(queue->lock));
}


void *dequeue(queue_t *queue) {
	void * ret;
	pthread_mutex_lock(&(queue->lock));

	while(queue->writes == queue->reads) {
		queue->reads = queue->writes = 0;
		pthread_cond_wait(&(queue->wait), &(queue->lock));
	}

	ret = queue->data[(queue->reads++) & (queue->size-1)];

	pthread_mutex_unlock(&(queue->lock));

	return ret;
}
