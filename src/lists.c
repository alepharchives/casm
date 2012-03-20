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

node* newString(char* str) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(data_node));
	strcpy(DATA(n)->getData.str, str);
	DATA(n)->offset=0;
	DATA(n)->kind = STRING_KIND;
	return n;
}

node* newAsmNode() {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(asm_node));
	ASM(n)->op_code=0;
	ASM(n)->word[0].val=0;
	ASM(n)->word[1].val=0;
	ASM(n)->word[2].val=0;
	ASM(n)->word[3].val=0;
	ASM(n)->word[0].type=a;
	ASM(n)->word[1].type=a;
	ASM(n)->word[2].type=a;
	ASM(n)->word[3].type=a;
	ASM(n)->size=0;
	ASM(n)->offset=0;
	return n;
}

node* newDeferedNode(int (*f)(list* l, addrVal*, char*, int, char*), list* l, addrVal* into, char* label, int lineNumber, char* origLine) {
	node* n = malloc(sizeof(node));
	defered_node* d = malloc(sizeof(defered_node));
	d->f = f;
	d->into=into;
	d->list=l;
	strcpy(d->label,label);
	d->lineNumber = lineNumber;
	strcpy(d->origLine, origLine);
	d->origLine[strlen(d->origLine)-1]='\0';
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
		case ASM_KIND: 		LABEL(n)->offset = LABEL(n)->get.code->offset; break;
		case DATA_KIND: 	LABEL(n)->offset = offset; offset+=LABEL(n)->get.data.getData.data.size; break;
		case STRING_KIND: 	LABEL(n)->offset = offset; offset+=strlen(LABEL(n)->get.data.getData.str); break;
		case NOT_INIT: if (LABEL(n)->isExtern!=1) {printf("Label %s referenced but not defined!\n", LABEL(n)->label); return -1;}break;
		}

		scan = &(*scan)->next;
	}
	return 0;
}

int execDeffered(list* l) {
	node** scan = l;
	node* n;
	int i=0;

	while (*scan!= NULL) {
		n = *scan;
		i |= INVOKE(n);
		scan = &(*scan)->next;
	}

	return i;
}

void intToBin(int i, char* out) {
	int j;
	for (j=0;j<16;j++) {
		out[15-j] = '0'+(i&1);
		i>>=1;
	}
	out[16]='\0';
}

void printOneAsm(asm_node* n) {
	char off[17], bits[17];
	int j;
	intToBin(n->offset, off);
	intToBin(n->op_code, bits);
	printf("offset %s (%d) : code %s a\n", off, n->offset, bits);
	for (j=1;j<n->size;j++) {
		int of = n->offset+j;
		intToBin(of, off);
		intToBin(n->word[j-1].val, bits);
		printf("offset %s (%d) : code %s %c\n", off, of, bits, n->word[j-1].type);
	}
}

void printAsm(list* l) {
	node** scan = l;
	node* n;

	while (*scan!= NULL) {
		n = *scan;
		printOneAsm(ASM(n));
		fflush(stdout);
		scan = &(*scan)->next;
	}
	return;
}

void printOneData(label_node* n) {
	char off[17], bits[17],l[50];
	int j;
	if (strlen(n->label)>0) sprintf(l, "(%s)", n->label);
	else l[0]='\0';
	if (n->kind==DATA_KIND) {
		for(j=0;j<n->get.data.getData.data.size;j++) {
			int of = n->offset+j;
			intToBin(of, off);
			intToBin( n->get.data.getData.data.nums[j],bits);
			printf("offset %s (%d) : data %s %s\n", off, of, bits, (j==0)?l:"");
		}
	} else if (n->kind == STRING_KIND) {
		for(j=0;j<=strlen(n->get.data.getData.str);j++) {
			int of = n->offset+j;
			intToBin(of, off);
			intToBin( n->get.data.getData.str[j],bits);
			printf("offset %s (%d) : data %s %s\n", off, of, bits, (j==0)?l:"");
		}
	}
}

void printData(list* l) {
	node** scan = l;
	node* n;

	while (*scan!= NULL) {
		n = *scan;
		printOneData(LABEL(n));
		fflush(stdout);
		scan = &(*scan)->next;
	}
	return;
}
