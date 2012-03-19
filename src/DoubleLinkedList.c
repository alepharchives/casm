#include <stdlib.h>
#include <stdio.h>
#include "DoubleLinkedList.h"

list insertHead(list l, node* new) {
	node** scan = &l;
	new->next = *scan;
	if (*scan != NULL)
		(*scan)->prev = new;
	*scan = new;
	return new;
}

list append(list l, node* new) {
	node** scan = &l, **head = &l;

	while (*scan != NULL) {
		new->prev = *scan;
		scan = &(*scan)->next;
	}
	new->next = *scan;
	if (*scan != NULL)
		(*scan)->prev = new;
	*scan = new;
	return *head;
}

node* getLast(list l) {
	node** scan = &l;

	while ((*scan)->next != NULL) {
		scan = &(*scan)->next;
	}

	return *scan;
}

list insertSorted(list l, node* new) {
	node** scan = &l, **head = &l;

	while (*scan != NULL && (*scan)->data < new->data) {
		new->prev = *scan;
		scan = &(*scan)->next;
	}
	new->next = *scan;
	if (*scan != NULL)
		(*scan)->prev = new;
	*scan = new;
	return *head;
}

void print(list l) {
	node** scan = &l;
	while (*scan != NULL) {
		printf("%s ", (char*)(*scan)->data);
		scan = &(*scan)->next;
	}
	printf("\n");
}

void printBackwards(list l) {
	node** scan = &l;
	while (*scan != NULL && (*scan)->next != NULL) {
		scan = &(*scan)->next;
	}

	while (*scan != NULL) {
		printf("%s ", (char*)(*scan)->data);
		scan = &(*scan)->prev;
	}
	printf("\n");
}

void freeList(list l) {
	node** scan = &l;
	while (*scan != NULL && (*scan)->next != NULL) {
		node *temp;
		temp = *scan;
		scan = &(*scan)->next;
		free(temp->data);
		free(temp);
	}
}

node* find(list l, int (*p)(node*, void*), void* data) {
	node** scan = &l;
	while (*scan != NULL && p(*scan, data)) scan = &(*scan)->next;
	return *scan;
}
