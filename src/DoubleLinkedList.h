#ifndef DDLINK
#define DDLINK

#include <stdlib.h>

typedef struct node {
	void* data;
	struct node* next, *prev;
} node, *list;

list insertHead(list l, node* new);
list insertSorted(list l,node* new);
void print(list l);
void freeList(list l);
void printBackwards(list l);
list append(list l, node* new);

#endif
