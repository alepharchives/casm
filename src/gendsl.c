/*
 * gendsl.c
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi
 */

#include  "gendsl.h"

void deferLabelAddrResolution(list* l, addrVal* into, char* label) {
	node* n = find(*l, findLabelText, label);
	if (n==NULL) {
		printf("label %s not defined!\n", label);
	} else {
		into->val = LABEL(n)->offset;
		into->type = LABEL(n)->isExtern ? e : r;
	}
}

void deferLabelDistanceResolution(list* l, addrVal* into, char* label) {
	into->val = 5;
	into->type=e;
}

void deferMakeSureLabelHasAddress(list* l, addrVal* into, char* label) {
	node* n = find(*l, findLabelText, label);
	if (n==NULL) {
		printf("label %s not defined!\n", label);
	} else if (LABEL(n)->offset==-1) {
		printf("label %s is not resolved!\n", label);
	}

}

void genExtern(char* label, Operand oper, Context* context) {
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	if (n!=NULL) {
		printf("label %s is already defined, cannot be extern.\n", oper.get.direct);
	} else {
		context->allLabels = append(context->allLabels, newExtern(oper.get.direct));
	}
}

void genEntry(char* label, Operand oper, Context* context) {
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	if (n==NULL) {
		n = newEntry(oper.get.direct);
		context->allLabels = append(context->allLabels, n);
		context->deferred = append(context->deferred, newDeferedNode(deferMakeSureLabelHasAddress, &context->allLabels, NULL, oper.get.direct));
	} else {
		LABEL(n)->isEntry=1;
	}
}

void genData(char* label, int* nums, int count, Context* context) {
	node* n;
	if (label[0]=='\0') {
		n = newLabel("", -1);
	} else {
		n = find(context->allLabels, findLabelText, label);
		if (n!=NULL) {
			printf("label %s already defined!\n", label);
			return;
		}
		else {
			n = newLabel(label, -1);
		}
	}
	LABEL(n)->kind = DATA_KIND;
	LABEL(n)->data.kind = DATA_KIND;
	LABEL(n)->data.nums=nums;
	LABEL(n)->data.size=count;
	LABEL(n)->data.offset=-1;
	context->allLabels = append(context->allLabels, n);
}

void genString(char* label, char* str, Context* context) {
	node* n;
	if (label[0]=='\0') {
		n = newLabel("", -1);
		context->allLabels = append(context->allLabels, n);
	} else {
		n = find(context->allLabels, findLabelText, label);
		if (n!=NULL) {
			printf("label %s already defined!\n", label);
			return;
		}
		else {
			n = newLabel(label, -1);
		}
	}
	LABEL(n)->kind = STRING_KIND;
	LABEL(n)->data.kind = STRING_KIND;
	LABEL(n)->data.str = str;
	LABEL(n)->data.offset=-1;
	context->allLabels = append(context->allLabels, n);
}

void asmLabel(Context* context, char* label) {
	node* n;
	node* asmNode;
	if (label[0]!='\0') {
		asmNode = getLast(context->codeList);
		n = find(context->allLabels, findLabelText, label);
		if (n==NULL) {
			n = newLabel(label, -1);
			LABEL(n)->kind = ASM_KIND;
			LABEL(n)->code = ASM(asmNode);
			context->allLabels = append(context->allLabels, n);
		} else if (LABEL(n)->kind == NOT_INIT){
			LABEL(n)->kind = ASM_KIND;
			LABEL(n)->code = ASM(asmNode);
		}
		else {
			printf("label %s already defined!\n", label);
		}
	}
}

GEN(mov_gen)
	START
		DO2(constant,	reg,	MOV(CONSTANT(source), 		REGISTER(dest)))
		/*DO2(constant,	direct, MOV(CONSTANT(source), 		DIRECT_LABEL(dest)))*/
else if (operand1.kind == constant && operand2.kind == direct){{ \
	OpCode code;\
	Operand* theOperand;\
	addrVal *theWord;\
	node* n;\
	byte size=1; \
	code.code=0;\
	code.bit.op=0;\
	theOperand=&operand1;\
	n  = newAsmNode(); \
	theWord = ((asm_node*)n->data)->word;\
	code.bit.sourceKind = theOperand->kind; /* CONSTANT */\
	code.bit.sourceReg  = 0;\
	theWord->val=theOperand->get.constant; \
	theWord->type=a; \
	size++;theWord++;; \
	theOperand=&operand2;\
	code.bit.destKind = theOperand->kind; /* DIRECT_LABEL*/ \
	code.bit.destReg  = 0;\
	context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,&context->allLabels, theWord, theOperand->get.direct));;\
	size++;theWord++;; \
	\
	context->codeList = append(context->codeList, n);\
	((asm_node*)n->data)->op_code = code.code; \
	((asm_node*)n->data)->size=size; \
	nothing(theWord);\
}}
		DO2(direct,		direct, MOV(DIRECT_LABEL(source), 	DIRECT_LABEL(dest)))
		DO2(direct, 	reg, 	MOV(DIRECT_LABEL(source), 	REGISTER(dest)))
		DO2(reg, 		direct, MOV(REGISTER(source), 		DIRECT_LABEL(dest)))
		DO2(reg, 		reg,	MOV(REGISTER(source), 		REGISTER(dest)))
	END(asmLabel(context, label))

GEN(cmp_gen)
	START
		DO2(constant,	reg,	CMP(CONSTANT(source), 		REGISTER(dest)))
		DO2(constant,	direct, CMP(CONSTANT(source), 		DIRECT_LABEL(dest)))
		DO2(direct,		direct, CMP(DIRECT_LABEL(source), 	DIRECT_LABEL(dest)))
		DO2(direct, 	reg, 	CMP(DIRECT_LABEL(source), 	REGISTER(dest)))
		DO2(reg, 		direct, CMP(REGISTER(source), 		DIRECT_LABEL(dest)))
		DO2(reg, 		reg,	CMP(REGISTER(source), 		REGISTER(dest)))
	END(asmLabel(context, label))

void nothing(addrVal* b){};
