/*
 * gendsl.h
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi
 */

#ifndef GENDSL_H_
#define GENDSL_H_

#include "asmdsl.h"
#include "DoubleLinkedList.h"

typedef unsigned char byte;

void deferLabelDistanceResolution(list* l, int* into, char* label);
void deferLabelAddrResolution(list* l, int* into, char* label);
node* newDeferedNode(void (*f)(list* l, int*, char*), list* l, int* into, char* label);
node* newAsmNode();
node* newLabel(char* label, int offset);
int findLabel(node* n, void* label);


typedef struct {
	int op_code, word1, word2;
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

typedef struct {
	char label[MAX_LABEL];
	int offset;
	int isExtern;
}label_node;


#define LABEL(n) ((label_node*)n->data)
#define ASM(n) ((asm_node*)n->data)
#define INVOKE(n) ((defered_node*)n->data)->f((((defered_node*)n->data)->list), (((defered_node*)n->data)->into), (((defered_node*)n->data)->label))


void (*f(byte*, char*));
typedef struct Foo {
    int flag    : 1;
    int counter : 15;
} Foo;

typedef
	union {
	int code;
	struct {
		int destReg:3;
		int destKind:3;
		int sourceReg:3;
		int sourceKind:3;
		int op:4;
	}bit;
} OpCode ;

#define REGISTER(T) \
	code.bit.T ## Kind = theOperand->kind; /* REGISTER */ \
	code.bit.T ## Reg  = theOperand->get.reg;

#define USE_EXTRA_WORD size++;theWord++;

#define CONSTANT(T)  \
	code.bit.T ## Kind = theOperand->kind; /* CONSTANT */\
	code.bit.T ## Reg  = 0;\
	**theWord=theOperand->get.constant; \
	USE_EXTRA_WORD

#define GET_LABEL_OFFSET(word) { \
		node* label = find(allLabels, findLabel, theOperand->get.direct); /* GET_LABEL_OFFSET */ \
		if (label == NULL) { \
			deferred = append(deferred, newDeferedNode(deferLabelAddrResolution,&allLabels, *word, theOperand->get.direct)); \
		} else **word = LABEL(label)->offset; \
	}


#define DIRECT_LABEL(T) \
	code.bit.T ## Kind = theOperand->kind; /* DIRECT_LABEL*/ \
	code.bit.T ## Reg  = 0;\
	GET_LABEL_OFFSET(theWord);\
	USE_EXTRA_WORD

#define MOV(OP1, OP2) { \
	OpCode code;\
	Operand* theOperand;\
	int *words[2];\
	int **theWord=words;\
	node* n;\
	byte size=1; \
	code.code=0;\
	code.bit.op=0;\
	theOperand=&operand1;\
	n  = newAsmNode(); \
	words[0]= &ASM(n)->word1;\
	words[1]= &ASM(n)->word2;\
	OP1; \
	theOperand=&operand2;\
	OP2; \
	\
	codeList = append(codeList, n);\
	ASM(n)->op_code = code.code; \
	ASM(n)->size=size; \
}



#endif /* GENDSL_H_ */
