/*
 * gendsl.c
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi.v
 *      Author: tal.a
 *
 * This file contains the implementation of the functions which actually generates the info into our AST
 */

#include  "gendsl.h"

/* A helper function for removing newlines, used for error reporting */
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

/* These function will resolve a label's location after the entire input file has been consumed */
int deferLabelAddrResolution(Context* l, addrVal* into, asm_node* asm_node,  char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
	if (n==NULL||(LABEL(n)->kind==NOT_INIT && !LABEL(n)->isExtern)) {
		printf(" Error at line %d '%s': label '%s' not defined.\n", lineNumber, trimNewline(origLine), label);
		return -1;
	} else {
		into->val = LABEL(n)->offset;
		into->type = LABEL(n)->isExtern ? e : r;

		/* in case we are dealing with an extern label, save its position for the .ext file */
		if(LABEL(n)->isExtern){
			int delta=1;
			/*int i;
			for (i=0;i<4 && (&asm_node->word[i] != into);i++) {
				delta++;
			}
			 */
			/* a cooler trick: */
			delta = into - &asm_node->word[0] + 1;
			l->externlabels = append(l->externlabels,newExEntWord(label,asm_node->offset+delta));
		}
	}
	return 0;
}

/* this function resolves the distance between a label and the last code offset of the program */
int  deferLabelDistanceResolution(Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
		if (n==NULL) {
			printf(" Error at line %d '%s': label '%s' not defined.\n", lineNumber, trimNewline(origLine), label);
			return -1;
		} else {
			/* last offset is the offset where the next op would be in,
			 * but our instructions are to point to the last offset,
			 * so we have to go back one word..*/

			if (LABEL(n)->isExtern) {
				/* if an extern label was used, notify the user that this cant be done. */
				printf(" Error at line %d '%s': cannot use extern label '%s' in an absolute form, defined at %d '%s'.\n", lineNumber, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				return -1;
			}
			into->val =  LABEL(n)->offset - (l->lastAsmOffset-1);
			into->type = a;
		}
	return 0;
}

/* This function is used to make sure entry labels has actually been defined somewhere, so if a label was not defined, an error message will be
 * printed to the user */
int  deferMakeSureEntryLabelHasAddress(Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
	if (n==NULL) {
		printf(" Error at line %d '%s': label '%s' not defined.\n", lineNumber,  trimNewline(origLine),  label);
		return -1;
	} else if (LABEL(n)->offset==-1) {
		printf(" Error at line %d '%s': label '%s' not resolved.\n", lineNumber,  trimNewline(origLine),  label);
		return -1;

	}
	return 0;
}

/* this function makes sure at the end that labels defined as extern have not been defined anywhere in the code */
int  deferMakeSureExternLabelNotDefined(Context* l, addrVal* into, asm_node* asm_node, char* label, int lineNumber, char* origLine) {
	node* n = find(l->allLabels, findLabelText, label);
	if (n==NULL) {
		printf(" Error at line %d '%s': label '%s' not defined.\n", lineNumber,  trimNewline(origLine),  label);
		return -1;
	} else if (LABEL(n)->offset != 0) {
		printf(" Error at line %d '%s': extern label '%s' is redefined in %d '%s'.\n", lineNumber,  trimNewline(origLine),  label,  LABEL(n)->origLineNuber, LABEL(n)->origLine);
		return -1;

	}
	return 0;
}

/*
 * the next *_gen functions are used specifically by the dot_try commands (.entry, .data, etc..)
 *
 * */

/* this function gets called when an .extern line was read, it makes sure that extern label was not defined before, and in any case,
 * it will queue a function that will makes sure at the end that it wasnt defined (deferMakeSureExternLabelNotDefined) */
void extern__gen(Context* context, Operand oper, char* label, int* err,  int lineNumber, char* originalLine){
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	if (n!=NULL) {
		printf(" Error at line %d '%s': extern label '%s' is already defined in line %d '%s'.\n", lineNumber,  trimNewline(originalLine),  oper.get.direct, LABEL(n)->origLineNuber, LABEL(n)->origLine);
		*err = GEN_ERROR;
	} else {
		context->allLabels = append(context->allLabels, newExtern(oper.get.direct, lineNumber, trimNewline(originalLine)));
		context->deferred  = append(context->deferred,  newDeferedNode(deferMakeSureExternLabelNotDefined, context, NULL, NULL, oper.get.direct, lineNumber, trimNewline(originalLine)));
	}
}

/* this function gets called when an .entry line was read, it makes sure that this label wasnt defined already as entry, and then
 * it takes not of it in the ast and calls a deferred function that will make sure the entry label was resolved. */
void entry_gen(Context* context, Operand oper, char* label, int* err,  int lineNumber, char* originalLine) {
	node* n = find(context->allLabels, findLabelText, oper.get.direct);
	originalLine = trimNewline(originalLine);

	/* if we didnt find a label with that name already, then create a new one, add it, and add a deferred function deferMakeSureEntryLabelHasAddress */
	if (n==NULL) {
		n = newEntry(oper.get.direct, lineNumber, originalLine);
		context->allLabels = append(context->allLabels, n);
		context->deferred = append(context->deferred, newDeferedNode(deferMakeSureEntryLabelHasAddress, context, NULL, NULL, oper.get.direct, lineNumber, originalLine));
	} else {
		/* it was already defined, makes sure it wasnt defined as entry before, otherwise show an error message*/
		if (LABEL(n)->isEntry==1) {
			printf(" Error at line %d '%s': label '%s' is already defined as entry.\n", lineNumber,  originalLine,  oper.get.direct);
			*err = GEN_ERROR;
			return;
		} else {
			/* it wasnt, so mark it as entry*/
			LABEL(n)->isEntry=1;
		}
	}
}

/* This function will receive data from a .data command only, meaning it will get an array nums which contains all the integer data (count of them) */
void data_gen(Context* context, char* label, int* err, int* nums, int count, int lineNum, char* origLine) {
	node* n;
	origLine = trimNewline(origLine);
	if (label[0]=='\0') {
		/* if this data has no name, so add a blank label - to keep it stored in the ast!*/
		n = newLabel("", lineNum, trimNewline(origLine));
		context->allLabels = append(context->allLabels, n);
	} else {
		n = find(context->allLabels, findLabelText, label);
		if (n!=NULL) {
			if (LABEL(n)->isExtern == 1) {
				printf(" Error at line %d '%s': label '%s' already defined as extern in line %d '%s'.\n", lineNum, origLine, label,  LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
			else if (LABEL(n)->isEntry == 0) {
				printf(" Error at line %d '%s': label '%s' already defined in line %d '%s'.\n", lineNum, origLine, label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
			else if (LABEL(n)->isEntry == 1) {
				LABEL(n)->origLineNuber = lineNum;
				strcpy(LABEL(n)->origLine, trimNewline(origLine));
			}
		}
		else {
			n = newLabel(label,  lineNum, origLine);
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
	} else {
		n = find(context->allLabels, findLabelText, label);
		if (n!=NULL) {
			if (LABEL(n)->isExtern == 1) {
				printf(" Error at line %d '%s': label '%s' defined as extern in line  %d '%s'.\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
			else if (LABEL(n)->isEntry == 0) {
				printf(" Error at line %d '%s': label '%s' already defined in line %d '%s'.\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
			else if (LABEL(n)->isEntry == 1) {
				LABEL(n)->origLineNuber = lineNum;
				strcpy(LABEL(n)->origLine, trimNewline(origLine));
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
			if (LABEL(n)->isExtern) {
				LABEL(n)->origLineNuber = lineNum;
				strcpy(LABEL(n)->origLine, trimNewline(origLine));
			}
		}
		else {
			if (LABEL(n)->isExtern) {
				printf(" Error at line %d '%s': label '%s' defined as extern in line %d '%s'.\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
			else {
				printf(" Error at line %d '%s': label '%s' already defined in line %d '%s'.\n", lineNum, trimNewline(origLine), label, LABEL(n)->origLineNuber, LABEL(n)->origLine);
				*err = GEN_ERROR;
				return;
			}
		}
	}
}

void nothing(addrVal* b){}
