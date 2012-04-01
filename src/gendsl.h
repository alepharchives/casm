/*
 * gendsl.h
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi.v
 *      Author: Tal.a
 *
 *  This file defines all the constructs that are used to actually generate the
 *  assembly code.
 *
 *  specifically look at ASSIGN2, its the most interesting!
 */

#ifndef GENDSL_H_
#define GENDSL_H_

#include "commons.h"
#include "asmdsl.h"
#include "lists.h"
#include "DoubleLinkedList.h"

/* error codes */
#define FIRST_ARG_IS_INVALID 4001
#define SECOND_ARG_IS_INVALID 4002
#define COMMAND_NOT_TERMINATED 4003
#define EMPTY_DATA 4004
#define BAD_DATA 4005
#define GEN_ERROR 4006

/* forward definitions of deferred functions */
int  deferLabelDistanceResolution(Context* l, addrVal* into, asm_node* asm_node,  char* label, int lineNumber, char* origLine);
int  deferLabelAddrResolution(Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine);

/*
 * This section defines the basic constructs that are composed together to create the functions that generatse the
 * assembly code.
 *
 * */

/* this asm command uses a register as one of its operands
 * notice the neat use of ## :) */
#define REGISTER(T) \
	code.bit.T ## Kind = theOperand->kind; /* REGISTER */ \
	code.bit.T ## Reg  = theOperand->get.reg;

/* this macro advances us to the next word */
#define USE_EXTRA_WORD size++;theWord++;

/* this asm command uses a constant as one of its operands */
#define CONSTANT(T)  \
	code.bit.T ## Kind = theOperand->kind; /* CONSTANT */\
	code.bit.T ## Reg  = 0;\
	theWord->val=theOperand->get.constant; \
	theWord->type=a; \
	USE_EXTRA_WORD

/* these macros are assigning deferred functions to resolve the corresponding labels at a later time */
#define GET_LABEL_OFFSET context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,context, theWord, ASM(n), theOperand->get.direct,lineNumber,originalLine));
#define GET_1D_LABEL_OFFSET context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,context, theWord, ASM(n), theOperand->get.oneIndex.label, lineNumber, originalLine));
#define GET_1D_INDEX_DISTANCE context->deferred = append(context->deferred, newDeferedNode(deferLabelDistanceResolution,context, theWord, ASM(n), theOperand->get.oneIndex.index, lineNumber, originalLine));
#define GET_2D_LABEL_OFFSET(P) context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,context, theWord, ASM(n), theOperand->get.twoIndice.P, lineNumber, originalLine));

/* this asm command uses a direct label as one of its operands */
#define DIRECT_LABEL(T) \
	code.bit.T ## Kind = theOperand->kind; /* DIRECT_LABEL*/ \
	code.bit.T ## Reg  = 0;\
	GET_LABEL_OFFSET;\
	USE_EXTRA_WORD

/* this asm command uses a one dimensional vector as one of its operands,
 * and it describes how to generate to proper code for it */
#define LABEL_1D(T) \
	code.bit.T ## Kind = theOperand->kind; /* LABEL_ONE_INDEX*/ \
	code.bit.T ## Reg  = 0; /* There is not register in this operand type */\
	GET_1D_LABEL_OFFSET;\
	USE_EXTRA_WORD;\
	GET_1D_INDEX_DISTANCE;\
	USE_EXTRA_WORD

/* this asm command uses a two dimensional matrix as one of its operands,
 * and it describes how to generate to proper code for it */
#define LABEL_2D(T) \
	code.bit.T ## Kind = theOperand->kind; /* LABEL_TWO_INDEX*/\
	code.bit.T ## Reg  = theOperand->get.twoIndice.reg;\
	GET_2D_LABEL_OFFSET(label);\
	USE_EXTRA_WORD;\
	GET_2D_LABEL_OFFSET(index);\
	USE_EXTRA_WORD

/* yeah, the compiler thinks we arent using these... silly compiler! */
#define REMOVE_WARNS2 theWord=&warn; warn++; *theWord++;

/* This macro defines the asm command for one that takes in two operands.
 * CODE is the number of that asm command (mov = 0, cmp = 1, etc..)
 * and OP1, OP2, are the composed code made out of the directives above */
#define OPER2(CODE, OP1, OP2) { \
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

/* This macro defines the asm command for one that takes in one operands.
 * CODE is the number of that asm command (mov = 0, cmp = 1, etc..)
 * and OP1, isthe composed code made out of the directives above */
#define OPER1(CODE, OP1) { \
	OpCode code;\
	Operand* theOperand;\
	addrVal *theWord;\
	node* n;\
	byte size=1; \
	code.code=0;\
	code.bit.op=CODE;\
	theOperand=&operand1;\
	n = newAsmNode(); \
	theWord = ASM(n)->word;\
	OP1; \
	context->codeList = append(context->codeList, n);\
	ASM(n)->op_code = code.code; \
	ASM(n)->size=size; \
	nothing(theWord);\
}

/* This macro defines the asm command for one that takes in no operands.
 * CODE is the number of that asm command (mov = 0, cmp = 1, etc..) */
#define OPER0(CODE) { \
	OpCode code;\
	addrVal *theWord;\
	node* n;\
	byte size=1; \
	code.code=0;\
	code.bit.op=CODE;\
	n  = newAsmNode(); \
	theWord = ASM(n)->word;\
	context->codeList = append(context->codeList, n);\
	ASM(n)->op_code = code.code; \
	ASM(n)->size=size; \
	nothing(theWord);\
}

/* helper directives for the generating functions */
#define GEN_CMD_2(cmd) void cmd(Context* context, Operand operand1, Operand operand2, char* label, int* err, int lineNumber, char* originalLine)
#define GEN_CMD_1(cmd) void cmd(Context* context, Operand operand1, char* label, int* err, int lineNumber, char* originalLine)
#define GEN_CMD_0(cmd) void cmd(Context* context, char* label, int* err, int lineNumber, char* originalLine)
#define START {if(0){}
#define DO2(kind1, kind2, op) else if (operand1.kind == kind1 && operand2.kind == kind2){op}
#define DO1(kind1, op) else if (operand1.kind == kind1){op}
#define END(code) code;}

/* */
#define ASSIGN2(name, CMD) GEN_CMD_2(name##_gen) \
	START \
		DO2(reg,					reg,								CMD(REGISTER(source), 			REGISTER(dest))) \
		DO2(reg,					constant,							CMD(REGISTER(source), 			CONSTANT(dest))) \
		DO2(reg,					direct, 							CMD(REGISTER(source), 			DIRECT_LABEL(dest))) \
		DO2(reg,					label_with_index,					CMD(REGISTER(source), 			LABEL_1D(dest))) \
		DO2(reg,					label_with_two_indices,				CMD(REGISTER(source), 			LABEL_2D(dest))) \
		DO2(constant,				reg,								CMD(CONSTANT(source), 			REGISTER(dest))) \
		DO2(constant,				constant,							CMD(CONSTANT(source), 			CONSTANT(dest))) \
		DO2(constant,				direct, 							CMD(CONSTANT(source), 			DIRECT_LABEL(dest))) \
		DO2(constant,				label_with_index,					CMD(CONSTANT(source), 			LABEL_1D(dest))) \
		DO2(constant,				label_with_two_indices,				CMD(CONSTANT(source), 			LABEL_2D(dest))) \
		DO2(direct,					reg,								CMD(DIRECT_LABEL(source), 		REGISTER(dest))) \
		DO2(direct,					constant,							CMD(DIRECT_LABEL(source), 		CONSTANT(dest))) \
		DO2(direct,					direct, 							CMD(DIRECT_LABEL(source), 		DIRECT_LABEL(dest))) \
		DO2(direct,					label_with_index,					CMD(DIRECT_LABEL(source), 		LABEL_1D(dest))) \
		DO2(direct,					label_with_two_indices,				CMD(DIRECT_LABEL(source), 		LABEL_2D(dest))) \
		DO2(label_with_index,		reg,								CMD(LABEL_1D(source), 			REGISTER(dest))) \
		DO2(label_with_index,		constant,							CMD(LABEL_1D(source), 			CONSTANT(dest))) \
		DO2(label_with_index,		direct, 							CMD(LABEL_1D(source), 			DIRECT_LABEL(dest))) \
		DO2(label_with_index,		label_with_index,					CMD(LABEL_1D(source), 			LABEL_1D(dest))) \
		DO2(label_with_index,		label_with_two_indices,				CMD(LABEL_1D(source), 			LABEL_2D(dest))) \
		DO2(label_with_two_indices,	reg,								CMD(LABEL_2D(source), 			REGISTER(dest))) \
		DO2(label_with_two_indices,	constant,							CMD(LABEL_2D(source), 			CONSTANT(dest))) \
		DO2(label_with_two_indices,	direct, 							CMD(LABEL_2D(source), 			DIRECT_LABEL(dest))) \
		DO2(label_with_two_indices,	label_with_index,					CMD(LABEL_2D(source), 			LABEL_1D(dest))) \
		DO2(label_with_two_indices,	label_with_two_indices,				CMD(LABEL_2D(source), 			LABEL_2D(dest))) \
	END(asmLabel(context, label, err, lineNumber, originalLine))

#define ASSIGN1(name, CMD) GEN_CMD_1(name##_gen) \
	START \
		DO1(reg,								CMD(REGISTER(dest))) \
		DO1(constant,							CMD(CONSTANT(dest))) \
		DO1(direct, 							CMD(DIRECT_LABEL(dest))) \
		DO1(label_with_index,					CMD(LABEL_1D(dest))) \
		DO1(label_with_two_indices,				CMD(LABEL_2D(dest))) \
	END(asmLabel(context, label, err, lineNumber, originalLine))

#define ASSIGN0(name, op) GEN_CMD_0(name##_gen){ {op} asmLabel(context, label, err, lineNumber, originalLine);}

void extern__gen(Context* context, Operand operand1, char* label, int* err, int lineNumber, char* originalLine);
void entry_gen(Context* context, Operand operand1, char* label, int* err, int lineNumber, char* originalLine);

void data_gen(Context* context, char* label, int* err, int* nums, int count, int lineNum, char* origLine);
void string_gen(Context* context, char* label, int* err,  char* str, int lineNum, char* origLine);

void asmLabel(Context* context, char* label, int* err, int lineNum, char* origLine);

void nothing(addrVal* i);

char* trimNewline(char* line);
#endif /* GENDSL_H_ */
