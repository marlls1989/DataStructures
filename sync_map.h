#ifndef SYNC_MAP_H
#define SYNC_MAP_H

#include "btree.h"
#include "rw_mutex.h"

typedef struct sync_map_t {
	rw_mutex_t lock;
	btree_t *btree;
	size_t count;
} sync_map_t;

typedef void (*map_walker_t)(void *val, char *key, void *arg);

void sync_map_init(sync_map_t *map);
void sync_map_destroy(sync_map_t *map, map_walker_t destructor, void *arg);

void* map_set(char *key, void *val, sync_map_t *map);
void* map_get(char *key, sync_map_t *map);
void* map_del(char *key, sync_map_t *map, map_walker_t destructor, void *arg);
void map_clear(sync_map_t *map, map_walker_t destructor, void *arg);
void map_iterate(sync_map_t *map, map_walker_t walker, void *arg);

#endif
