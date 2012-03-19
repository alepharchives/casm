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

#define FIRST_ARG_IS_INVALID 4001
#define SECOND_ARG_IS_INVALID 4002

void deferLabelDistanceResolution(list* l, addrVal* into, char* label);
void deferLabelAddrResolution(list* l, addrVal* into, char* label);

#define REGISTER(T) \
	code.bit.T ## Kind = theOperand->kind; /* REGISTER */ \
	code.bit.T ## Reg  = theOperand->get.reg;

#define USE_EXTRA_WORD size++;theWord++;

#define CONSTANT(T)  \
	code.bit.T ## Kind = theOperand->kind; /* CONSTANT */\
	code.bit.T ## Reg  = 0;\
	theWord->val=theOperand->get.constant; \
	theWord->type=a; \
	USE_EXTRA_WORD

#define GET_LABEL_OFFSET context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,&context->allLabels, theWord, theOperand->get.direct)); \

#define GET_ONE_INDEX_LABEL_OFFSET context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,&context->allLabels, theWord, theOperand->get.oneIndex.label));

/*#define GET_DISTANCE_LABEL_OFFSET context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,&context->allLabels, theWord, theOperand->get.oneIndex.label));*/

#define DIRECT_LABEL(T) \
	code.bit.T ## Kind = theOperand->kind; /* DIRECT_LABEL*/ \
	code.bit.T ## Reg  = 0;\
	GET_LABEL_OFFSET;\
	USE_EXTRA_WORD

#define LABEL_ONE_INDEX(T) \
	code.bit.T ## Kind = theOperand->kind; /* LABEL_ONE_INDEX*/ \
	code.bit.T ## Reg  = 0;\
	GET_ONE_INDEX_LABEL_OFFSET;\
	USE_EXTRA_WORD


#define REMOVE_WARNS2 theWord=&warn; warn++; *theWord++;

#define OPER(CODE, OP1, OP2) { \
	OpCode code;\
	Operand* theOperand;\
	addrVal *theWord;\
	node* n;\
	byte size=1; \
	code.code=0;\
	code.bit.op=CODE;\
	theOperand=&operand1;\
	n  = newAsmNode(); \
	theWord = ASM(n)->word;\
	OP1; \
	theOperand=&operand2;\
	OP2; \
	\
	context->codeList = append(context->codeList, n);\
	ASM(n)->op_code = code.code; \
	ASM(n)->size=size; \
	nothing(theWord);\
}

#define MOV(OP1, OP2) OPER(0,OP1,OP2)
#define CMP(OP1, OP2) OPER(1,OP1,OP2)

#define GEN(cmd) void cmd(Context* context, Operand operand1, Operand operand2, char* label)
#define START {if(0){}
#define DO2(kind1, kind2, op) else if (operand1.kind == kind1 && operand2.kind == kind2){op}
#define DO1(kind1, op) else if (operand1.kind == kind1){op}
#define END(code) code;}


void mov_gen(Context* context, Operand operand1, Operand operand2, char* label);
void genExtern(char* label, Operand oper, Context* context);
void genEntry(char* label, Operand oper, Context* context);
void genData(char* label, int* nums, int count, Context* context);
void genString(char* label, char* str, Context* context);

void nothing(addrVal* i);
#endif /* GENDSL_H_ */
