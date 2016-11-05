#include <stdlib.h>
#include <stdio.h>

#include "sync_map.h"

void sync_map_init(sync_map_t *map) {
	rw_mutex_init(&(map->lock));
	map->btree = NULL;
	map->count = 0;
}

static void _btree_destruction_walk(btree_t *b, map_walker_t destructor, void *arg) {
	if(!b) return;
	
	_btree_destruction_walk(b->left, destructor, arg);
	free(b->left);
	_btree_destruction_walk(b->right, destructor, arg);
	free(b->right);
	
	if(destructor)
		destructor(b->val, b->key, arg);
}

void map_clear(sync_map_t *map, map_walker_t destructor, void *arg) {
	write_lock(&(map->lock));
	_btree_destruction_walk(map->btree, destructor, arg);
	free(map->btree);
	map->count = 0;
	write_unlock(&(map->lock));
}

void sync_map_destroy(sync_map_t *map, map_walker_t destructor, void *arg) {
	map_clear(map, destructor, arg);

	rw_mutex_destroy(&(map->lock));
}

void *map_set(char *key, void *val, sync_map_t *map) {
	btree_t *node;
	void *ret = NULL;
	
	write_lock(&(map->lock));
	
	if((node = btree_search(key, map->btree))) {
		ret = node->val;
		node->val = val;
	} else {
		node = malloc(sizeof(btree_t));
		node->left = node->right = NULL;
		node->key = key;
		node->val = val;
		btree_insert(node, &(map->btree));
		map->count++;
	}

	write_unlock(&(map->lock));

	return ret;
}

void *map_get(char *key, sync_map_t *map) {
	btree_t *node;
	void *ret;
	read_lock(&(map->lock));

	node = btree_search(key, map->btree);
	ret = node ? node->val : NULL;
	
	read_unlock(&(map->lock));

	return ret;
}

void *map_del(char *key, sync_map_t *map, map_walker_t destructor, void *arg) {
	btree_t *node;
	void *ret = NULL;
	write_lock(&(map->lock));

	if((node = btree_splice(key, &(map->btree)))) {
		if(destructor)
			destructor(node->val, node->key, arg);

		map->count--;
		ret = node->val;
		free(node);
	}
	
	write_unlock(&(map->lock));

	return ret;
}

static void _btree_walker(map_walker_t walker, void *arg, btree_t *n) {
	if(!n) return;
	
	_btree_walker(walker, arg,  n->left);
	walker(n->val, n->key, arg);
	_btree_walker(walker, arg, n->right);
}

void map_iterate(sync_map_t *map, map_walker_t walker, void *arg) {
	read_lock(&(map->lock));
	_btree_walker(walker, arg, map->btree);
	read_unlock(&(map->lock));
}
