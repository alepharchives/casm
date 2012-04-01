#ifndef DDLINK
#define DDLINK

#include <stdlib.h>

/* the node's structure */
typedef struct node {
	void* data;
	struct node* next, *prev;
} node, *list;

/* straight forward O(1) head insertion */
list insertHead(list l, node* new);

/* look for a node using a specific predicate function */
node* find(list l, int (node*, void*), void*);

/* insert the item into its sorted order. works only for ordered types */
list insertSorted(list l,node* new);

/* prints the list */
void print(list l);

/* backward printing */
void printBackwards(list l);

/* straight forward O(n) tail insertion */
list append(list l, node* new);

/* get the last element */
node* getLast(list l);

#endif
