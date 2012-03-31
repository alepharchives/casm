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
	if (n==NULL||(LABEL(n)->kind==NOT_INIT && !LABEL(n)->isExtern)) {
		printf("Error at line %d '%s': label '%s' not defined!\n", lineNumber, trimNewline(origLine), label);
		return -1;
	} else {
		into->val = LABEL(n)->offset;
		into->type = LABEL(n)->isExtern ? e : r;

		if(LABEL(n)->isExtern){
			int i;
			int delta=1;
			for (i=0;i<4 && (&asm_node->word[i] != into);i++) {
				delta++;
			}
			l->externlabels = append(l->externlabels,newExEntWord(label,asm_node->offset+delta));
		}
	}
	return 0;
}

int  deferLabelDistanceResolution(Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
		if (n==NULL) {
			printf("Error at line %d '%s': label '%s' not defined!\n", lineNumber, trimNewline(origLine), label);
			return -1;
		} else {
			/* last offset is the offset where the next op would be in,
			 * but our instructions are to point to the last offset,
			 * so we have to go back one word..*/

			if (LABEL(n)->isExtern) {
				printf("Error at line %d '%s': cannot use extern label '%s' in an absolute form, defined at %d '%s'.\n", lineNumber, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				return -1;
			}
			into->val =  LABEL(n)->offset - (l->lastAsmOffset-1);
			into->type = a;
		}
	return 0;
}

int  deferMakeSureLabelHasAddress(Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
	if (n==NULL) {
		printf("Error at line %d '%s': label '%s' not defined!\n", lineNumber,  trimNewline(origLine),  label);
		return -1;
	} else if (LABEL(n)->offset==-1) {
		printf("Error at line %d '%s': label '%s' not resolved!\n", lineNumber,  trimNewline(origLine),  label);
		return -1;

	}
	return 0;
}

void extern__gen(Context* context, Operand oper, char* label, int* err,  int lineNumber, char* originalLine){
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	if (n!=NULL) {
		printf("Error at line %d '%s': extern label '%s' is already defined in line %d '%s'.\n", lineNumber,  trimNewline(originalLine),  oper.get.direct, LABEL(n)->origLineNuber, LABEL(n)->origLine);
		*err = GEN_ERROR;
	} else {
		context->allLabels = append(context->allLabels, newExtern(oper.get.direct, lineNumber, trimNewline(originalLine)));
	}
}

void entry_gen(Context* context, Operand oper, char* label, int* err,  int lineNumber, char* originalLine) {
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	originalLine = trimNewline(originalLine);
	if (n==NULL) {
		n = newEntry(oper.get.direct, lineNumber, originalLine);
		context->allLabels = append(context->allLabels, n);
		context->deferred = append(context->deferred, newDeferedNode(deferMakeSureLabelHasAddress, context, NULL, NULL, oper.get.direct, lineNumber, originalLine));
	} else {
		if (LABEL(n)->isEntry==1) {
			printf("Error at line %d '%s': label '%s' is already defined as entry.\n", lineNumber,  trimNewline(originalLine),  oper.get.direct);
			*err = GEN_ERROR;
			return;
		} else {
			LABEL(n)->isEntry=1;
		}
	}
}

void data_gen(Context* context, char* label, int* err, int* nums, int count, int lineNum, char* origLine) {
	node* n;
	if (label[0]=='\0') {
		n = newLabel("", lineNum, trimNewline(origLine));
		context->allLabels = append(context->allLabels, n);
	} else {
		n = find(context->allLabels, findLabelText, label);
		if (n!=NULL) {
			if (LABEL(n)->isExtern == 1) {
				printf("Error at line %d '%s': label '%s' already defined as extern in line %d '%s'\n", lineNum, trimNewline(origLine), label,  LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
			else if (LABEL(n)->isEntry == 0) {
				printf("Error at line %d '%s': label '%s' already defined in line %d '%s'\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
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

void string_gen(Context* context, char* label, int* err, char* str, int lineNum, char* origLine) {
	node* n;
	if (label[0]=='\0') {
		n = newLabel("",  lineNum, trimNewline(origLine));
		context->allLabels = append(context->allLabels, n);
		context->allLabels = append(context->allLabels, n);
	} else {
		n = find(context->allLabels, findLabelText, label);
		if (n!=NULL) {
			if (LABEL(n)->isExtern == 1) {
				printf("Error at line %d '%s': label '%s' defined as extern in line  %d '%s' \n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
			else if (LABEL(n)->isEntry == 0) {
				printf("Error at line %d '%s': label '%s' already defined in line %d '%s'\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
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

void asmLabel(Context* context, char* label, int* err, int lineNum, char* origLine) {
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
			if (LABEL(n)->isEntry) {
				LABEL(n)->origLineNuber = lineNum;
				strcpy(LABEL(n)->origLine, trimNewline(origLine));
			}
		}
		else {
			if (LABEL(n)->isExtern) {
				printf("Error at line %d '%s': label '%s' defined as extern in line %d '%s'\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
			else {
				printf("Error at line %d '%s': label '%s' already defined in line %d '%s'\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
		}
	}
}

void nothing(addrVal* b){}
