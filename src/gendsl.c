/*
 * gendsl.c
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi
 */

#include  "gendsl.h"

void deferLabelAddrResolution(list* l, int* into, char* label) {
	node* n = find(*l, findLabelText, label);
	if (n==NULL) {
		printf("label %s not defined!", label);
	} else {
		*into = LABEL(n)->offset;
	}
}

void deferLabelDistanceResolution(list* l, int* into, char* label) {
	*into = 5;
}

void deferMakeSureLabelHasAddress(list* l, int* into, char* label) {
	node* n = find(*l, findLabelText, label);
	if (n==NULL) {
		printf("label %s not defined!\n", label);
	} else if (LABEL(n)->offset==-1) {
		printf("label %s is not resolved!\n", label);
	}

}

void genExtern(char* label, Operand oper, Context* context) {
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	if (n==NULL) {
		printf("label %s is already defined, cannot be extern.", oper.get.direct);
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

	LABEL(n)->data.kind = DATA_KIND;
	LABEL(n)->data.get.nums=nums;
	context->allLabels = append(context->allLabels, n);
}

/*ASSIGN2(movs, mov, MOV(REGISTER(source), DIRECT_LABEL(dest)), reg, direct)*/
void movregdirect(Context* context, Operand operand1, Operand operand2, char* label) {
	OpCode code;
	Operand* theOperand;
	int *theWord;
	node* n;
	byte size=1;
	code.code=0;
	code.bit.op=0;
	theOperand=&operand1;
	n  = newAsmNode();
	theWord = ((asm_node*)n->data)->word;
	code.bit.sourceKind = theOperand->kind; /* REGISTER */
	code.bit.sourceReg  = theOperand->get.reg;;
	theOperand=&operand2;
	code.bit.destKind = theOperand->kind; /* DIRECT_LABEL*/
	code.bit.destReg  = 0;
	{
		node* label = find(context->allLabels, findLabelText, theOperand->get.direct); /* GET_LABEL_OFFSET */
		if (label == ((void *)0)) {
			context->deferred = append(context->deferred, newDeferedNode(deferLabelAddrResolution,&context->allLabels, theWord, theOperand->get.direct));
		} else *theWord = ((label_node*)label->data)->offset;
	};
	size++;theWord++;;

	context->codeList = append(context->codeList, n);
	((asm_node*)n->data)->op_code = code.code;
	((asm_node*)n->data)->size=size;
}
void(*movs[5][4])(Context*, Operand, Operand, char*);

void init(void(*fs[5][4])(Context*, Operand, Operand, char*), void(*f)(Context*, Operand, Operand, char*), addr from, addr to) {
fs[from][to]=f;
}
