#include <stdlib.h>
#include <stdio.h>
#include "DoubleLinkedList.h"

void insertHead(node* new) {
	node** scan = &root;
	new->next = *scan;
	if (*scan != NULL)
		(*scan)->prev = new;
	*scan = new;
}

void insertSorted(node* new) {
	node** scan = &root;
	while (*scan != NULL && (*scan)->data < new->data) {
		new->prev = *scan;
		scan = &(*scan)->next;
	}
	new->next = *scan;
	if (*scan != NULL)
		(*scan)->prev = new;
	*scan = new;
}

void print() {
	node** scan = &root;
	while (*scan != NULL) {
		printf("%d ", (*scan)->data);
		scan = &(*scan)->next;
	}
	printf("\n");
}

void printBackwards() {
	node** scan = &root;
	while (*scan != NULL && (*scan)->next != NULL) {
		scan = &(*scan)->next;
	}

	while (*scan != NULL) {
		printf("%d ", (*scan)->data);
		scan = &(*scan)->prev;
	}
	printf("\n");
}

void freeList() {
	node** scan = &root;
	while (*scan != NULL && (*scan)->next != NULL) {
		node *temp;
		temp = *scan;
		scan = &(*scan)->next;
		free(temp);
	}

}