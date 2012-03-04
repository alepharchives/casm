#ifndef DDLINK
#define DDLINK

#include <stdlib.h>

typedef struct node {
	int data;
	struct node* next, *prev;
} node;

node* root;

void insertHead(node* new);
void insertSorted(node* new);
void print();
void freeList();
void printBackwards();

#endif
