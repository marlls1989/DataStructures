#ifndef RW_MUTEX_H
#define RW_MUTEX_H

#include <pthread.h>

typedef struct {
	pthread_mutex_t lock;
	pthread_cond_t readersQ;
	pthread_cond_t writersQ;
	volatile unsigned readers;
	volatile unsigned writers;
	volatile unsigned waiting_writers;
} rw_mutex_t;

void rw_mutex_init(rw_mutex_t *mux);
void rw_mutex_destroy(rw_mutex_t *mux);
void read_lock(rw_mutex_t *mux);
void read_unlock(rw_mutex_t *mux);
void write_lock(rw_mutex_t *mux);
void write_unlock(rw_mutex_t *mux);

#endif
