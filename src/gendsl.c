/*
 * gendsl.c
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi
 */

#include  "gendsl.h"

int findLabel(node* n, void* label) {
	return strcmp(LABEL(n)->label, (char*)label);
}

node* newLabel(char* label, int offset) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(label_node));
	strcpy(LABEL(n)->label, label);
	LABEL(n)->offset=offset;
	return n;
}

node* newAsmNode() {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(asm_node));
	ASM(n)->op_code=0;
	ASM(n)->word1=0;
	ASM(n)->word2=0;
	ASM(n)->size=0;
	ASM(n)->offset=0;
	return n;
}

node* newDeferedNode(void (*f)(list* l, int*, char*), list* l, int* into, char* label) {
	node* n = malloc(sizeof(node));
	defered_node* d = malloc(sizeof(defered_node));
	d->f = f;
	d->into=into;
	d->list=l;
	strcpy(d->label,label);
	n->prev=NULL;
	n->data=d;
	return n;
}

void deferLabelAddrResolution(list* l, int* into, char* label) {
	node* n = find(*l, findLabel, label);
	if (n==NULL) {
		printf("label %s not defined!", label);
	} else {
		*into = LABEL(n)->offset;
	}
}

void deferLabelDistanceResolution(list* l, int* into, char* label) {
	*into = 5;
}
