#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "btree.h"

// Pretty stupid btree implementation, should implement selfbalancing for any real propose

btree_t *btree_search(char* key, btree_t *root) {
	int a;
	
	if(root) {
		a = strcmp(root->key, key);
		return a < 0 ? btree_search(key, root->left) :
			a > 0 ? btree_search(key, root->right) : root;
	}

	return NULL;
}

int btree_insert(btree_t *node, btree_t **root) {
	int a = 0;
	btree_t **c = root;

	if(!node)
		return 0;

	while(*c)
		if((a = strcmp((*c)->key, node->key)) < 0)
			c = &((*c)->left);
		else if (a > 0)
			c = &((*c)->right);
		else
			return 0;

	*c = node;

	return a;
}

btree_t *btree_splice(char *key, btree_t **root_ref) {
	btree_t **c = root_ref;
	btree_t *n;
	int a;

	while(*c && (a = strcmp((*c)->key, key)))
		if(a < 0)
			c = &((*c)->left);
		else
			c = &((*c)->right);

	if(!(n = *c))
		return NULL;

	*c = NULL;
	btree_insert(n->left, root_ref);
	btree_insert(n->right, root_ref);

	return n;
}

