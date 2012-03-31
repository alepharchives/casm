/*
 * gendsl.c
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi
 */

#include  "gendsl.h"

char* trimNewline(char* line)
{
	int l = strlen(line);
	int i=0;
	if (oneOf(&line[l-1],"\r\n")) {
		i++;
		if (charIs(&line[l-2],'\r')) i++;
	}
	line[l-i]='\0';
	return line;
}

int deferLabelAddrResolution(Context* l, addrVal* into, asm_node* asm_node,  char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
	int i;
	if (n==NULL||(LABEL(n)->kind==NOT_INIT && !LABEL(n)->isExtern)) {
		printf("Error at line %d '%s': label %s not defined!\n", lineNumber, trimNewline(origLine), label);
		return -1;
	} else {
		into->val = LABEL(n)->offset;
		i = LABEL(n)->offset;
		into->type = LABEL(n)->isExtern ? e : r;

		if(LABEL(n)->isExtern){
			int delta=1;
			for (i=0;i<4 && (&asm_node->word[i] !=  into);i++) {
				delta++;
			}

			i = asm_node->offset+delta;

			l->externlabels = append(l->externlabels,newExEntWord(label,i));
		}
	}
	return 0;
}

int  deferLabelDistanceResolution(Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
		if (n==NULL) {
			printf("Error at line %d '%s': label %s not defined!\n", lineNumber, trimNewline(origLine), label);
			return -1;
		} else {
			/* last offset is the offset where the next op would be in,
			 * but our instructions are to point to the last offset,
			 * so we have to go back one word..*/
			into->val =  LABEL(n)->offset - (l->lastAsmOffset-1);
			into->type = a;
		}
	return 0;
}

int  deferMakeSureLabelHasAddress(Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
	if (n==NULL) {
		printf("Error at line %d '%s': label %s not defined!\n", lineNumber,  trimNewline(origLine),  label);
		return -1;
	} else if (LABEL(n)->offset==-1) {
		printf("Error at line %d '%s': label %s not resolved!\n", lineNumber,  trimNewline(origLine),  label);
		return -1;

	}
	return 0;
}

void extern__gen(Context* context, Operand oper, char* label, int lineNumber, char* originalLine){
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	if (n!=NULL) {
		printf("Error at line %d '%s': label %s is already defined in line %d '%s', cannot be extern.\n", lineNumber,  trimNewline(originalLine),  oper.get.direct, LABEL(n)->origLineNuber, LABEL(n)->origLine);
	} else {
		context->allLabels = append(context->allLabels, newExtern(oper.get.direct, lineNumber, trimNewline(originalLine)));
	}
}

void entry_gen(Context* context, Operand oper, char* label, int lineNumber, char* originalLine) {
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	originalLine = trimNewline(originalLine);
	if (n==NULL) {
		n = newEntry(oper.get.direct, lineNumber, originalLine);
		context->allLabels = append(context->allLabels, n);
		context->deferred = append(context->deferred, newDeferedNode(deferMakeSureLabelHasAddress, context, NULL, NULL, oper.get.direct, lineNumber, originalLine));
	} else {
		LABEL(n)->isEntry=1;
	}
}

void data_gen(Context* context, char* label, int* nums, int count, int lineNum, char* origLine) {
	node* n;
	if (label[0]=='\0') {
		n = newLabel("", lineNum, trimNewline(origLine));
		context->allLabels = append(context->allLabels, n);
	} else {
		n = find(context->allLabels, findLabelText, label);
		if (n!=NULL) {
			if (LABEL(n)->isExtern == 1) {
				printf("Error at line %d '%s': label '%s' defined as extern!\n", lineNum, trimNewline(origLine), label);
				return;
			}
			else if (LABEL(n)->isEntry == 0) {
				printf("Error at line %d '%s': label '%s' already defined in line %d '%s'\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				return;
			}
		}
		else {
			n = newLabel(label,  lineNum, trimNewline(origLine));
			context->allLabels = append(context->allLabels, n);
		}
	}
	LABEL(n)->kind = DATA_KIND;
	LABEL(n)->get.data.kind = DATA_KIND;
	LABEL(n)->get.data.getData.data.nums=nums;
	LABEL(n)->get.data.getData.data.size=count;
	LABEL(n)->get.data.offset=-1;
}

void string_gen(Context* context, char* label, char* str, int lineNum, char* origLine) {
	node* n;
	if (label[0]=='\0') {
		n = newLabel("",  lineNum, trimNewline(origLine));
		context->allLabels = append(context->allLabels, n);
		context->allLabels = append(context->allLabels, n);
	} else {
		n = find(context->allLabels, findLabelText, label);
		if (n!=NULL) {
			if (LABEL(n)->isExtern == 1) {
				printf("Error at line %d '%s': label '%s' defined as extern!\n", lineNum, trimNewline(origLine), label);
				return;
			}
			else if (LABEL(n)->isEntry == 0) {
				printf("Error at line %d '%s': label '%s' already defined in line %d '%s'\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				return;
			}
		}
		else {
			n = newLabel(label,   lineNum, trimNewline(origLine));
			context->allLabels = append(context->allLabels, n);
		}
	}
	LABEL(n)->kind = STRING_KIND;
	LABEL(n)->get.data.kind = STRING_KIND;
	LABEL(n)->get.data.getData.str = str;
	LABEL(n)->get.data.offset=-1;

}

void asmLabel(Context* context, char* label,int lineNum, char* origLine) {
	node* n;
	node* asmNode;
	if (label[0]!='\0') {
		asmNode = getLast(context->codeList);
		n = find(context->allLabels, findLabelText, label);
		if (n==NULL) {
			n = newLabel(label,  lineNum, trimNewline(origLine));
			LABEL(n)->kind = ASM_KIND;
			LABEL(n)->get.code = ASM(asmNode);
			context->allLabels = append(context->allLabels, n);
		} else if (LABEL(n)->kind == NOT_INIT){
			LABEL(n)->kind = ASM_KIND;
			LABEL(n)->get.code = ASM(asmNode);
		}
		else {
			if (LABEL(n)->isExtern == 1) {
				printf("Error at line %d '%s': label '%s' defined as extern!\n", lineNum, trimNewline(origLine), label);
				return;
			}
			else if (LABEL(n)->isEntry == 0) {
				printf("Error at line %d '%s': label '%s' already defined!\n", lineNum, trimNewline(origLine), label);
				return;
			}
		}
	}
}

void nothing(addrVal* b){}
