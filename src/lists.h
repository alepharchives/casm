/*
 * lists.h
 *
 *  Created on: 18 במרס 2012
 *      Author: ypsw
 */
#ifndef LISTS_H
#define LISTS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commons.h"
#include "DoubleLinkedList.h"

node* newDeferedNode(void (*f)(list* l, int*, char*), list* l, int* into, char* label);
node* newAsmNode();
node* newLabel(char* label, int offset);
node* newExtern(char* label);
node* newEntry(char* label);
node* newData(int* nums);

int findLabelText(node* n, void* label);
int findLabelEntry(node* n, void* label);
int findLabelExtern(node* n, void* label);


typedef struct {
	int op_code, word[4];
	/*char op*/
	byte size;
	int offset;
} asm_node ;

typedef struct {
	void (*f)(list* l, int*, char*);
	int* into;
	list* list;
	char label[MAX_LABEL];
}defered_node;

#define STRING_KIND 1
#define DATA_KIND 2

typedef struct {
	byte kind;
	union {
		int* nums;
		char* str;
	} get;
	int offset;
} data_node;

typedef struct {
	char label[MAX_LABEL];
	int offset;
	byte isExtern;
	byte isEntry;
	union {
		asm_node code;
		data_node data;
	};
}label_node;

#define LABEL(n) ((label_node*)n->data)
#define ASM(n) ((asm_node*)n->data)
#define DATA(n) ((data_node*)n->data)

#define INVOKE(n) ((defered_node*)n->data)->f((((defered_node*)n->data)->list), (((defered_node*)n->data)->into), (((defered_node*)n->data)->label))

#endif
