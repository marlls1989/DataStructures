#ifndef BTREE_H
#define BTREE_H

typedef struct btree_t {
	char *key;
  void *val;
  struct btree_t *left, *right;
} btree_t;


btree_t *btree_search(char *key, btree_t *root);
int btree_insert(btree_t *node, btree_t **root);
btree_t *btree_splice(char *key, btree_t **root_ref);

#endif
