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

int  deferLabelDistanceResolution(list* l, addrVal* into, char* label, int lineNumber, char* origLine);
int  deferLabelAddrResolution(list* l, addrVal* into, char* label, int lineNumber, char* origLine);

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

#define GET_LABEL_OFFSET context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,&context->allLabels, theWord, theOperand->get.direct,lineNumber,originalLine)); \


#define DIRECT_LABEL(T) \
	code.bit.T ## Kind = theOperand->kind; /* DIRECT_LABEL*/ \
	code.bit.T ## Reg  = 0;\
	GET_LABEL_OFFSET;\
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

#define GEN(cmd) void cmd(Context* context, Operand operand1, Operand operand2, char* label, int lineNumber, char* originalLine)
#define START {if(0){}
#define DO2(kind1, kind2, op) else if (operand1.kind == kind1 && operand2.kind == kind2){op}
#define DO1(kind1, op) else if (operand1.kind == kind1){op}
#define END(code) code;}


GEN(mov_gen);
GEN(cmp_gen);

void genExtern(char* label, Operand oper, Context* context, int lineNum, char* origLine);
void genEntry(char* label, Operand oper, Context* context, int lineNum, char* origLine);
void genData(char* label, int* nums, int count, Context* context, int lineNum, char* origLine);
void genString(char* label, char* str, Context* context, int lineNum, char* origLine);

void nothing(addrVal* i);
#endif /* GENDSL_H_ */
