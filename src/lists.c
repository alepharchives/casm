/*
 * lists.h
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi.v
 *      Author: Tal.a
 *
 * This file contains the implementation of the functions that handles our variety of out lists
 */


#include "lists.h"

/* Returns the find function for each type of nodes */
int findLabelText(node* n, void* label) {
	return strcmp(LABEL(n)->label, (char*)label);
}

int findLabelExEntText(node* n, void* label) {
	return strcmp(EXENT(n)->label, (char*)label);
}

int findLabelEntry(node* n, void* label) {
	return LABEL(n)->isEntry==1 && strcmp(LABEL(n)->label, (char*)label) ;
}

int findLabelExtern(node* n, void* label) {
	return LABEL(n)->isExtern==1 && strcmp(LABEL(n)->label, (char*)label) ;
}


/* Create new general label node for out list and initialize the node parameters */
node* _newLabel(char* label, int offset, byte isExtern, byte isEntry,int origLineCount, char* origLine ) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(label_node));
	strcpy(LABEL(n)->label, label);
	LABEL(n)->kind = NOT_INIT;
	LABEL(n)->offset=offset;
	LABEL(n)->isEntry=isEntry;
	LABEL(n)->isExtern=isExtern;
	LABEL(n)->origLineNuber = origLineCount;
	strcpy(LABEL(n)->origLine, origLine);

	return n;
}

/* Create a specific label node using the general create function */
node* newLabel(char* label, int origLineCount, char* origLine) {
	return _newLabel(label, -1, 0,0, origLineCount, origLine);
}

node* newExtern(char* label, int origLineCount, char* origLine) {
	return _newLabel(label,0,1,0, origLineCount, origLine);
}

node* newEntry(char* label, int origLineCount, char* origLine) {
	return _newLabel(label,-1,0,1, origLineCount, origLine);
}

/* Create a new string node */
node* newString(char* str) {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(data_node));
	strcpy(DATA(n)->getData.str, str);
	DATA(n)->offset=0;
	DATA(n)->kind = STRING_KIND;
	return n;
}

/* Create new Extern Entry node for the list that saves them for the output .ext, .ent files  */
node* newExEntWord(char* label, int offset){
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(exent_node));
	strcpy(EXENT(n)->label, label);
	EXENT(n)->offset = offset;
	return n;
}

/* Create a new Asm node that initialize with default parameters */
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

/* Create a node to the defer list */
node* newDeferedNode(int (*f)(Context* l, addrVal*, asm_node* , char*, int, char*), Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine) {
	node* n = malloc(sizeof(node));
	defered_node* d = malloc(sizeof(defered_node));
	d->f = f;
	d->into=into;
	d->cont=l;
	strcpy(d->label,label);
	d->lineNumber = lineNumber;
	strcpy(d->origLine, origLine);
	n->prev=NULL;
	n->data=d;
	d->asmNode = asm_node;
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

/*  */
int computeLabelOffset(list* l, int lastAsmOffset) {
	node** scan = l, *n;
	int offset = lastAsmOffset;

	while (*scan!= NULL) {
		n = *scan;
		switch (LABEL(n)->kind) {
		case ASM_KIND: 		LABEL(n)->offset = LABEL(n)->get.code->offset; break;
		case DATA_KIND: 	LABEL(n)->offset = offset; offset+=LABEL(n)->get.data.getData.data.size; break;
		case STRING_KIND: 	LABEL(n)->offset = offset; offset+=strlen(LABEL(n)->get.data.getData.str)+1; break;
		case NOT_INIT: break;/*if (LABEL(n)->isExtern!=1) {printf("error in line %d: in line '%s' Label %s referenced but not defined!\n", LABEL(n)->origLineNuber, LABEL(n)->origLine, LABEL(n)->label); return -1;}*/break;
		}

		scan = &(*scan)->next;
	}
	return offset;
}

/* Invoke the function that was given to the defer node to get the information for the asm node
 * in the "second transfer" */
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

/* Gets an Int type and returns the binary value in string format */
void intToBin(int i, char* out) {
	int j;
	for (j=0;j<20;j++) {
		if (j==3 || j==7 || j==11 ||j==15) out[19-j] = ' ';
		else {
			out[19-j] = '0'+(i&1);
			i>>=1;
		}
	}
	out[20]='\0';
}

/* The same but with 8 bits */
void intToBin8(int i, char* out) {
	int j;
	for (j=0;j<8;j++) {
		out[7-j] = '0'+(i&1);
		i>>=1;
	}
	out[8]='\0';
}

/* 4 bits */
void intToBin4(int i, char* out) {
	int j;
	for (j=0;j<4;j++) {
		out[3-j] = '0'+(i&1);
		i>>=1;
	}
	out[4]='\0';
}


/*  */
void printOneAsm(asm_node* n) {
	OffsetBits off;
	AsmBits bits;
	int j;
	intToBin8(n->offset, off);
	intToBin(n->op_code, bits);
	printf("offset %s (%d) : code %s a\n", off, n->offset, bits);
	for (j=1;j<n->size;j++) {
		int of = n->offset+j;
		intToBin8(of, off);
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

/* Get a single label node and a file pointer, the function writes to the files all the data of this
 * specific node*/
void printOneDataToFile(label_node* n, FILE *f) {
	OffsetBits off;
	AsmBits bits;
	char l[50];
	int j;
	if (strlen(n->label)>0) sprintf(l, "(%s)", n->label);
	else l[0]='\0';
	if (n->kind==DATA_KIND) {
		for(j=0;j<n->get.data.getData.data.size;j++) {
			int of = n->offset+j;
			intToBin8(of, off);
			intToBin( n->get.data.getData.data.nums[j],bits);
			fprintf(f, "%s   %s\n", off ,bits);
		}
	} else if (n->kind == STRING_KIND) {
		for(j=0;j<=strlen(n->get.data.getData.str);j++) {
			int of = n->offset+j;
			intToBin8(of, off);
			intToBin( n->get.data.getData.str[j],bits);
			fprintf(f, "%s   %s\n", off ,bits);
		}
	}
}

void printOneData(label_node* n) {
	OffsetBits off;
	AsmBits bits;
	char l[50];
	int j;
	if (strlen(n->label)>0) sprintf(l, "(%s)", n->label);
	else l[0]='\0';
	if (n->kind==DATA_KIND) {
		for(j=0;j<n->get.data.getData.data.size;j++) {
			int of = n->offset+j;
			intToBin8(of, off);
			intToBin( n->get.data.getData.data.nums[j],bits);
			printf("offset %s (%d) : data %s %s\n", off, of, bits, (j==0)?l:"");
		}
	} else if (n->kind == STRING_KIND) {
		for(j=0;j<=strlen(n->get.data.getData.str);j++) {
			int of = n->offset+j;
			intToBin8(of, off);
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

/* Gets an asm node with all data include and write it to the file pointer for the obj file. */
void writeOneAsmToFile(asm_node* n, FILE *f) {
	OffsetBits off;
	AsmBits bits;
	int j;
	intToBin8(n->offset, off);
	intToBin(n->op_code, bits);
	fprintf(f ,"%s   %s a\n", off, bits);
	/* Run over all the size of current command in the asm node */
	for (j=1;j<n->size;j++) {
		int of = n->offset+j;
		intToBin8(of, off);
		intToBin(n->word[j-1].val, bits);
		fprintf(f, "%s   %s %c\n", off, bits, n->word[j-1].type);
	}
}

/* The function creates obj file using the lists of the context and the writeOneAsm,
 * printOneDataToFilewrite functions  */
void writeAsm(Context* c, FILE *f) {
	node** scan = &c->codeList;
	node** scan2 = &c->allLabels;
	node* n;
	node* n2;
	OffsetBits  lastoff;
	OffsetBits lastdataoff;
	int delta = c->lastDataOffset - c->lastAsmOffset;

	/* Calculate the size of the code and the data for the first line in obj file */
	intToBin8(c->lastAsmOffset - ASM(c->codeList)->offset, lastoff);
	if (delta < 16) intToBin4(delta, lastdataoff);
	else intToBin8(delta, lastdataoff);

	fprintf(f,"%s %s \n",lastoff, lastdataoff);

	/* Write all the code information */
	while (*scan!= NULL) {
		n = *scan;
		writeOneAsmToFile(ASM(n), f);
		scan = &(*scan)->next;
	}

	/* Write all the data details */
	while (*scan2!= NULL) {
		n2 = *scan2;
		printOneDataToFile(LABEL(n2),f);
		scan2 = &(*scan2)->next;
		}

	return;
}

/* Writes a single node data to the ent\ext files */
void writeOneExEnt(exent_node* n, FILE *f){
	char offset[20];
	intToBin8(n->offset,offset);
	fprintf(f,"%s   %s \n",n->label, offset);
}

/* Writes the Extern\Entry files */
void writeExEnt(list* l, FILE *f) {
	node** scan = l;
	node* n;

	while (*scan!= NULL) {
		n = *scan;
		writeOneExEnt(EXENT(n), f);
		fflush(stdout);
		scan = &(*scan)->next;
	}
	return;
}

/* Write to file the enrty parameters from the list */
int extractEntries(list* l,FILE *f) {
	node** scan = l, *n;
	OffsetBits off;

	while (*scan!= NULL) {
		n = *scan;
		if (LABEL(n)->isEntry) {
			intToBin8(LABEL(n)->offset,off);
			fprintf(f,"%s   %s \n",LABEL(n)->label, off);
		}
		scan = &(*scan)->next;
	}
	return 0;
}

/* Free the context saved memory */
void freeContext(Context* context) {

	node** scan = &context->allLabels;
	while (*scan != NULL) {
		node *temp;
		temp = *scan;
		*scan = (*scan)->next;
		if (LABEL(temp)->kind == DATA_KIND) {
			free(LABEL(temp)->get.data.getData.data.nums);
		}
		else if (LABEL(temp)->kind == STRING_KIND) {
			free(LABEL(temp)->get.data.getData.str);
		}
		free(temp->data);
		free(temp);
	}

	scan = &context->deferred;
	while (*scan != NULL) {
		node *temp;
		temp = *scan;
		*scan = (*scan)->next;
		free(temp->data);
		free(temp);
	}

	scan = &context->externlabels;
	while (*scan != NULL) {
		node *temp;
		temp = *scan;
		*scan = (*scan)->next;
		free(temp->data);
		free(temp);
	}

	scan = &context->codeList;
	while (*scan != NULL) {
		node *temp;
		temp = *scan;
		*scan = (*scan)->next;
		free(temp->data);
		free(temp);
	}

}
