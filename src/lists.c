#include "lists.h"

int findLabelText(node* n, void* label) {
	return strcmp(LABEL(n)->label, (char*)label);
}

int findLabelEntry(node* n, void* label) {
	return LABEL(n)->isEntry==1 && strcmp(LABEL(n)->label, (char*)label) ;
}

int findLabelExtern(node* n, void* label) {
	return LABEL(n)->isExtern==1 && strcmp(LABEL(n)->label, (char*)label) ;
}

node* _newLabel(char* label, int offset, byte isExtern, byte isEntry) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(label_node));
	strcpy(LABEL(n)->label, label);
	LABEL(n)->kind = NOT_INIT;
	LABEL(n)->offset=offset;
	LABEL(n)->isEntry=isEntry;
	LABEL(n)->isExtern=isExtern;
	return n;
}

node* newLabel(char* label, int offset) {
	return _newLabel(label, offset, 0,0);
}

node* newExtern(char* label) {
	return _newLabel(label,0,1,0);
}

node* newEntry(char* label) {
	return _newLabel(label,-1,0,1);
}

/*
node* newData(int* nums) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(data_node));
	memcpy(DATA(n)->nums, nums);
	DATA(n)->offset=0;
	DATA(n)->kind = DATA_KIND;
	return n;
}*/

node* newString(char* str) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(data_node));
	strcpy(DATA(n)->str, str);
	DATA(n)->offset=0;
	DATA(n)->kind = STRING_KIND;
	return n;
}

node* newAsmNode() {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(asm_node));
	ASM(n)->op_code=0;
	ASM(n)->word[0]=0;
	ASM(n)->word[1]=0;
	ASM(n)->word[2]=0;
	ASM(n)->word[3]=0;
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

/* This function will set all the offsets of the AST, and return the last offset */
int computeAsmOffset(list* l, int initial) {
	node** scan = l;
	node* n;
	int offset = initial;

	while (*scan!= NULL) {
		n = *scan;
		ASM(n)->offset = offset;
		offset += ASM(n)->size;
		scan = &(*scan)->next;
	}
	return offset;
}

int computeLabelOffset(list* l, int lastAsmOffset) {
	node** scan = l, *n;
	int offset = lastAsmOffset;

	while (*scan!= NULL) {
		n = *scan;
		switch (LABEL(n)->kind) {
		case ASM_KIND: LABEL(n)->offset = LABEL(n)->code->offset; break;
		case DATA_KIND: LABEL(n)->offset = offset; offset+=LABEL(n)->data.size;break;
		case STRING_KIND: LABEL(n)->offset = offset; offset+=strlen(LABEL(n)->data.str);break;
		case NOT_INIT: printf("Label %s referenced but not defined!\n", LABEL(n)->label); return -1;
		}

		scan = &(*scan)->next;
	}
	return 0;
}
