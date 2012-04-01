/*
 * commons.h
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi.v
 *
 *  This file implements a double linked list. I never really use its double feature,
 *  and most of the methods here are never used, just wrote it to practice usage
 *  of double pointers before the test.. :)
 *
 *  this is the main structure used for maintaining the assembler's ast.
 *  also, this is a sort of a "generic" form of list, where we made special
 *  accessor macros to to the type transformation (check out list.h)
 */

#include <stdlib.h>
#include <stdio.h>
#include "DoubleLinkedList.h"

/* straight forward O(1) head insertion */
list insertHead(list l, node* new) {
	node** scan = &l;
	new->next = *scan;
	if (*scan != NULL)
		(*scan)->prev = new;
	*scan = new;
	return new;
}

/* straight forward O(n) tail insertion */
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

/* get the last element */
node* getLast(list l) {
	node** scan = &l;

	while ((*scan)->next != NULL) {
		scan = &(*scan)->next;
	}

	return *scan;
}

/* insert the item into its sorted order. works only for ordered types */
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

/* prints the list */
void print(list l) {
	node** scan = &l;
	while (*scan != NULL) {
		printf("%s ", (char*)(*scan)->data);
		scan = &(*scan)->next;
	}
	printf("\n");
}

/* backward printing */
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

/* look for a node using a specific predicate function */
node* find(list l, int (*p)(node*, void*), void* data) {
	node** scan = &l;
	while (*scan != NULL && p(*scan, data)) scan = &(*scan)->next;
	return *scan;
}
