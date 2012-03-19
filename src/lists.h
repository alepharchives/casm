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
/*node* newData(int* nums);*/

int findLabelText(node* n, void* label);
int findLabelEntry(node* n, void* label);
int findLabelExtern(node* n, void* label);
int computeAsmOffset(list* l, int initial);
int computeLabelOffset(list* l, int lastAsmOffset);
void execDeffered(list* l);
void printAsm(list* l);
void printData(list* l);

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

typedef enum {
	STRING_KIND,
	DATA_KIND ,
	ASM_KIND ,
	NOT_INIT
} label_kind;

typedef struct {
	label_kind kind;
	union {
		struct {
			int* nums;
			int size;
		};
		char* str;
	};
	int offset;
} data_node;

typedef struct {
	char label[MAX_LABEL];
	int offset;
	byte isExtern;
	byte isEntry;
	label_kind kind;
	union {
		asm_node* code;
		data_node data;
	};
}label_node;

#define LABEL(n) ((label_node*)n->data)
#define ASM(n) ((asm_node*)n->data)
#define DATA(n) ((data_node*)n->data)

#define INVOKE(n) ((defered_node*)n->data)->f((((defered_node*)n->data)->list), (((defered_node*)n->data)->into), (((defered_node*)n->data)->label))

#endif
