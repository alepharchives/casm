/*
 * gendsl.c
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi
 */

#include  "gendsl.h"

int deferLabelAddrResolution(list* l, addrVal* into, char* label, int lineNumber, char* origLine) {
	node* n = find(*l, findLabelText, label);
	if (n==NULL) {
		printf("Error at line %d '%s': label %s not defined!\n", lineNumber, origLine, label);
		return -1;
	} else {
		into->val = LABEL(n)->offset;
		into->type = LABEL(n)->isExtern ? e : r;
	}
	return 0;
}

int  deferLabelDistanceResolution(list* l, addrVal* into, char* label, int lineNumber, char* origLine) {
	into->val = 5;
	into->type=e;
	return 0;
}

int  deferMakeSureLabelHasAddress(list* l, addrVal* into, char* label, int lineNumber, char* origLine) {
	node* n = find(*l, findLabelText, label);
	if (n==NULL) {
		printf("Error at line %d '%s': label %s not defined!\n", lineNumber, origLine, label);
		return -1;
	} else if (LABEL(n)->offset==-1) {
		printf("Error at line %d '%s': label %s not resolved!\n", lineNumber, origLine, label);
		return -1;
	}
	return 0;
}

void genExtern(char* label, Operand oper, Context* context,int lineNum, char* origLine) {
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	if (n!=NULL) {
		printf("label %s is already defined, cannot be extern.\n", oper.get.direct);
	} else {
		context->allLabels = append(context->allLabels, newExtern(oper.get.direct));
	}
}

void genEntry(char* label, Operand oper, Context* context, int lineNum, char* origLine) {
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	if (n==NULL) {
		n = newEntry(oper.get.direct);
		context->allLabels = append(context->allLabels, n);
		context->deferred = append(context->deferred, newDeferedNode(deferMakeSureLabelHasAddress, &context->allLabels, NULL, oper.get.direct, lineNum, origLine));
	} else {
		LABEL(n)->isEntry=1;
	}
}

void genData(char* label, int* nums, int count, Context* context, int lineNum, char* origLine) {
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
	LABEL(n)->get.data.kind = DATA_KIND;
	LABEL(n)->get.data.getData.data.nums=nums;
	LABEL(n)->get.data.getData.data.size=count;
	LABEL(n)->get.data.offset=-1;
	context->allLabels = append(context->allLabels, n);
}

void genString(char* label, char* str, Context* context, int lineNum, char* origLine) {
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
	LABEL(n)->get.data.kind = STRING_KIND;
	LABEL(n)->get.data.getData.str = str;
	LABEL(n)->get.data.offset=-1;
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
			LABEL(n)->get.code = ASM(asmNode);
			context->allLabels = append(context->allLabels, n);
		} else if (LABEL(n)->kind == NOT_INIT){
			LABEL(n)->kind = ASM_KIND;
			LABEL(n)->get.code = ASM(asmNode);
		}
		else {
			printf("label %s already defined!\n", label);
		}
	}
}

GEN(mov_gen)
	START

		DO2(constant,				reg,	MOV(CONSTANT(source), 		REGISTER(dest)))
		DO2(constant,				direct, MOV(CONSTANT(source), 		DIRECT_LABEL(dest)))
		DO2(direct,					direct, MOV(DIRECT_LABEL(source), 	DIRECT_LABEL(dest)))
		DO2(direct, 				reg, 	MOV(DIRECT_LABEL(source), 	REGISTER(dest)))
		DO2(reg, 					direct, MOV(REGISTER(source), 		DIRECT_LABEL(dest)))
		DO2(reg, 					reg,	MOV(REGISTER(source), 		REGISTER(dest)))

	/*	DO2(label_with_two_indices, reg,	MOV(LABEL_2D(source), 		REGISTER(dest)))*/
		/*DO2(reg, 		reg,	MOV(REGISTER(source), 		REGISTER(dest)))*/

	END(asmLabel(context, label))

GEN(cmp_gen)
	START
		DO2(constant,				reg,			CMP(CONSTANT(source), 		REGISTER(dest)))
		DO2(constant,				direct, 		CMP(CONSTANT(source), 		DIRECT_LABEL(dest)))
		DO2(direct,					direct, 		CMP(DIRECT_LABEL(source), 	DIRECT_LABEL(dest)))
		DO2(direct, 				reg, 			CMP(DIRECT_LABEL(source), 	REGISTER(dest)))
		DO2(reg, 					direct, 		CMP(REGISTER(source), 		DIRECT_LABEL(dest)))

	END(asmLabel(context, label))

void nothing(addrVal* b){};
























