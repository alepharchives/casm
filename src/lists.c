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

node* newEntry(char* label, int offset) {
	return _newLabel(label,offset,0,1);
}

node* newData(int* nums) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(data_node));
	strcpy(DATA(n)->get.nums, nums);
	DATA(n)->offset=0;
	DATA(n)->kind = DATA_KIND;
	return n;
}

node* newString(char* str) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(data_node));
	strcpy(DATA(n)->get.str, str);
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
