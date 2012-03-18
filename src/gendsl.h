/*
 * gendsl.h
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi
 */

#ifndef GENDSL_H_
#define GENDSL_H_

#include "commons.h"
#include "asmdsl.h"
#include "lists.h"
#include "DoubleLinkedList.h"



void deferLabelDistanceResolution(list* l, int* into, char* label);
void deferLabelAddrResolution(list* l, int* into, char* label);




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
	*theWord=theOperand->get.constant; \
	USE_EXTRA_WORD

#define GET_LABEL_OFFSET { \
		node* label = find(allLabels, findLabelText, theOperand->get.direct); /* GET_LABEL_OFFSET */ \
		if (label == NULL) { \
			deferred = append(deferred, newDeferedNode(deferLabelAddrResolution,&allLabels, theWord, theOperand->get.direct)); \
		} else *theWord = LABEL(label)->offset; \
	}


#define DIRECT_LABEL(T) \
	code.bit.T ## Kind = theOperand->kind; /* DIRECT_LABEL*/ \
	code.bit.T ## Reg  = 0;\
	GET_LABEL_OFFSET;\
	USE_EXTRA_WORD

#define MOV(OP1, OP2) { \
	OpCode code;\
	Operand* theOperand;\
	int *theWord;\
	node* n;\
	byte size=1; \
	code.code=0;\
	code.bit.op=0;\
	theOperand=&operand1;\
	n  = newAsmNode(); \
	theWord = ASM(n)->word;\
	OP1; \
	theOperand=&operand2;\
	OP2; \
	\
	codeList = append(codeList, n);\
	ASM(n)->op_code = code.code; \
	ASM(n)->size=size; \
}



#endif /* GENDSL_H_ */
