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
	LABEL(n)->kind = DATA_KIND;
	LABEL(n)->data.kind = DATA_KIND;
	LABEL(n)->data.nums=nums;
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

void mov_gen(Context* context, Operand operand1, Operand operand2, char* label) {

	switch (operand1.kind) {
	case constant:
		switch (operand2.kind) {
		case direct:				MOV(CONSTANT(source), DIRECT_LABEL(dest))	break;
		case label_with_index:  			break;
		case label_with_two_indices: 		break;
		case reg: 					MOV(CONSTANT(source), REGISTER(dest)) break;
		}
		break;
	case direct:
		switch (operand2.kind) {
		case direct:				MOV(DIRECT_LABEL(source), DIRECT_LABEL(dest)) break;
		case label_with_index: break;
		case label_with_two_indices: break;
		case reg:					MOV(DIRECT_LABEL(source), REGISTER(dest)) break;
		}
		break;
	case label_with_index:
		switch (operand2.kind) {
		case direct:break;
		case label_with_index:break;
		case label_with_two_indices:break;
		case reg:break;
		}
		break;
	case label_with_two_indices:
		switch (operand2.kind) {
		case direct:break;
		case label_with_index:break;
		case label_with_two_indices:break;
		case reg:break;
		}
		break;
	case reg:
		switch (operand2.kind) {
		case direct:				MOV(REGISTER(source), DIRECT_LABEL(dest)) break;
		case label_with_index:break;
		case label_with_two_indices:break;
		case reg:					MOV(REGISTER(source), REGISTER(dest)) break;
		}
		break;
	}
	asmLabel(context, label);
}
