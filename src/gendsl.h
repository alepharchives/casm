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
#define COMMAND_NOT_TERMINATED 4003
#define EMPTY_DATA 4004
#define BAD_DATA 4005


int  deferLabelDistanceResolution(Context* l, addrVal* into, char* label, int lineNumber, char* origLine);
int  deferLabelAddrResolution(Context* l, addrVal* into, char* label, int lineNumber, char* origLine);

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

#define GET_LABEL_OFFSET context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,context, theWord, theOperand->get.direct,lineNumber,originalLine)); \

#define GET_1D_LABEL_OFFSET context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,context, theWord, theOperand->get.oneIndex.label, lineNumber, originalLine)); \

#define GET_1D_INDEX_DISTANCE context->deferred = append(context->deferred, newDeferedNode(deferLabelDistanceResolution,context, theWord, theOperand->get.oneIndex.index, lineNumber, originalLine)); \

#define GET_2D_LABEL_OFFSET(P) context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,context, theWord, theOperand->get.twoIndice.P, lineNumber, originalLine));

#define DIRECT_LABEL(T) \
	code.bit.T ## Kind = theOperand->kind; /* DIRECT_LABEL*/ \
	code.bit.T ## Reg  = 0;\
	GET_LABEL_OFFSET;\
	USE_EXTRA_WORD

#define LABEL_1D(T) \
	code.bit.T ## Kind = theOperand->kind; /* LABEL_ONE_INDEX*/ \
	code.bit.T ## Reg  = 0; /* There is not register in this operand type */\
	GET_1D_LABEL_OFFSET;\
	USE_EXTRA_WORD;\
	GET_1D_INDEX_DISTANCE;\
	USE_EXTRA_WORD

#define LABEL_2D(T) \
	code.bit.T ## Kind = theOperand->kind; /* LABEL_TWO_INDEX*/\
	code.bit.T ## Reg  = theOperand->get.twoIndice.reg;\
	GET_2D_LABEL_OFFSET(label);\
	USE_EXTRA_WORD;\
	GET_2D_LABEL_OFFSET(index);\
	USE_EXTRA_WORD

#define REMOVE_WARNS2 theWord=&warn; warn++; *theWord++;

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

#define OPER1(CODE, OP1) { \
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
	context->codeList = append(context->codeList, n);\
	ASM(n)->op_code = code.code; \
	ASM(n)->size=size; \
	nothing(theWord);\
}

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


#define GEN_CMD_2(cmd) void cmd(Context* context, Operand operand1, Operand operand2, char* label, int lineNumber, char* originalLine)
#define GEN_CMD_1(cmd) void cmd(Context* context, Operand operand1, char* label, int lineNumber, char* originalLine)
#define GEN_CMD_0(cmd) void cmd(Context* context, char* label, int lineNumber, char* originalLine)
#define START {if(0){}
#define DO2(kind1, kind2, op) else if (operand1.kind == kind1 && operand2.kind == kind2){op}
#define DO1(kind1, op) else if (operand1.kind == kind1){op}
#define END(code) code;}

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
	END(asmLabel(context, label, lineNumber, originalLine))

#define ASSIGN1(name, CMD) GEN_CMD_1(name##_gen) \
	START \
		DO1(reg,								CMD(REGISTER(dest))) \
		DO1(constant,							CMD(CONSTANT(dest))) \
		DO1(direct, 							CMD(DIRECT_LABEL(dest))) \
		DO1(label_with_index,					CMD(LABEL_1D(dest))) \
		DO1(label_with_two_indices,				CMD(LABEL_2D(dest))) \
	END(asmLabel(context, label, lineNumber, originalLine))

#define ASSIGN0(name, op) GEN_CMD_0(name##_gen){ {op} asmLabel(context, label, lineNumber, originalLine);}

void extern__gen(Context* context, Operand operand1, char* label, int lineNumber, char* originalLine);
void entry_gen(Context* context, Operand operand1, char* label, int lineNumber, char* originalLine);

void data_gen(Context* context, char* label, int* nums, int count, int lineNum, char* origLine);
void string_gen(Context* context, char* label, char* str, int lineNum, char* origLine);

void asmLabel(Context* context, char* label,int lineNum, char* origLine);

void nothing(addrVal* i);

char* trimNewline(char* line);
#endif /* GENDSL_H_ */
