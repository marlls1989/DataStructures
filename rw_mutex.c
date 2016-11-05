#include <pthread.h>
#include <stdio.h>

#include "rw_mutex.h"

void rw_mutex_init(rw_mutex_t *mux) {
	mux->readers =
		mux->writers =
		mux->waiting_writers = 0;

	pthread_mutex_init(&(mux->lock), NULL);
	pthread_cond_init(&(mux->readersQ), NULL);
	pthread_cond_init(&(mux->writersQ), NULL);
}

void rw_mutex_destroy(rw_mutex_t *mux) {
	pthread_cond_destroy(&(mux->writersQ));
	pthread_cond_destroy(&(mux->readersQ));
	pthread_mutex_destroy(&(mux->lock));
}

void read_lock(rw_mutex_t *mux) {
	pthread_mutex_lock(&(mux->lock));
	
	while(mux->writers)
		pthread_cond_wait(&(mux->readersQ), &(mux->lock));

	
	mux->readers++;
	pthread_mutex_unlock(&(mux->lock));
}

void read_unlock(rw_mutex_t *mux) {
	pthread_mutex_lock(&(mux->lock));
	mux->readers--;
	pthread_mutex_unlock(&(mux->lock));
}

void write_lock(rw_mutex_t *mux) {
	pthread_mutex_lock(&(mux->lock));
	mux->waiting_writers++;

	
	while(mux->readers || mux->writers)
		pthread_cond_wait(&(mux->writersQ), &(mux->lock));

	
	mux->writers++;
	pthread_mutex_unlock(&(mux->lock));
}

void write_unlock(rw_mutex_t *mux) {
	pthread_mutex_lock(&(mux->lock));

	mux->writers--;
	if(--(mux->waiting_writers))
		pthread_cond_signal(&(mux->writersQ));
	else
		pthread_cond_broadcast(&(mux->readersQ));


	pthread_mutex_unlock(&(mux->lock));
	
}
