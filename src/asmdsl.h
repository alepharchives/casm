/*
 * asdDsl.h
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */

#ifndef ASMDSL_H_
#define ASMDSL_H_

#include "commons.h"
#include "parser.h"
#include "DoubleLinkedList.h"
#include "lists.h"
#include "gendsl.h"

#define DO_(c) \
		int count=c;		 \
		Operand a[c] ;		    \
		int pi=0,i;			\
		Operand val;			\
		val.kind = none; \
		for (i=0;i<count;i++) { a[i].kind=none;}

#define FIN_OR(code) \
		for (i=0;val.kind==none&&i<count;i++) {val=a[i];} \
		if (val.kind != none) { \
			code;			\
		};

#define REMOVE_WARNS pi=val.get.constant;pi++;pi=a[0].get.constant; /* a silly line just to remove annoying "unused" warnings*/
#define HANDLE_ERROR if (*err!=0) return NULL;

#define OR2(A,B)   \
    /*OR2*/ 		    \
    DO_(2)		        \
    l = NULL;			\
	A;B;				\
	l = NULL;

#define OR3(A,B,C) \
    /*OR3*/ 		    \
    DO_(3)		        \
    l = NULL;			\
	A;B;C;				\
	l = NULL;

#define OR4(A,B,C,D)   \
    /*OR3*/ 		    \
    DO_(4)		        \
    l = NULL;			\
	A;B;C;D;			\
	l = NULL;

#define OR5(A,B,C,D,E)   \
    /*OR3*/ 		    \
    DO_(5)		        \
    l = NULL;			\
	A;B;C;D;E;			\
	l = NULL;

#define VAL(i) theVals[i-1]

#define DIRECT \
	if (l==NULL) /*DIRECT*/ { \
		l = strip(getAllAlphasDigits(strip(p," "), a[pi].get.direct), " "); \
		l = oneOf(l,",\n\r"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = direct; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;

#define LABEL1 \
	if (l==NULL) /*LABEL1*/ { \
		l = charIs(getAllAlphasDigits(charIs(charIs(getAllAlphasDigits(strip(p," "), a[pi].get.oneIndex.label), '[') ,'%'), a[pi].get.oneIndex.index), ']'); \
		l = oneOf(l,",\n\r"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = label_with_index; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;

#define LABEL2 \
	if (l==NULL) /*LABEL2*/ { \
		char r; \
		l = charIs(getAllAlphasDigits(charIs(strip(p, " "), '['), a[pi].get.twoIndice.index), ']'); \
		l = getAllAlphasDigits(l, a[pi].get.twoIndice.label); \
		l = charIs(getDigit(charIs(charIs(l, '['), 'r'), &r), ']'); \
		l = oneOf(l,",\n\r"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = label_with_two_indices; \
				a[pi].get.twoIndice.reg = r - '0'; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;

#define REG  \
	if (l==NULL) /*REG*/ { \
		char* n; \
		l = charIs(strip(p," "), 'r'); \
			if (l!=NULL) { \
				if ((n=oneOf(strip(isDigit(l), " "),",\n\r"))) { \
					a[pi].kind = reg; \
					a[pi].get.reg = 0; \
					a[pi].get.reg = (char)*(l)-'0'; \
					val = a[pi]; \
					p=n;\
				} \
				else l = NULL; \
		}\
	}\
	pi++;


#define CONST  \
	if (l==NULL) /*CONST*/ { \
		l = charIs(strip(p," "),'#');\
		if (l!=NULL) { \
			l = oneOf(getInteger(strip(l," "),&a[pi].get.constant, err), ",\n\r"); \
			if (l!=NULL) { \
				a[pi].kind = constant; \
				val = a[pi];\
				p=l; \
			} \
		}\
	}\
    pi++;

#define VERIFY { \
	if (*p!='\n' && *p!='\0' && *p!='\r') { \
		*err=COMMAND_NOT_TERMINATED; \
		return NULL; \
	} \
}

#define TWO(A,B,code) { \
	Operand theVals[2],val; /*TWO*/	\
	Operand a[2];\
	int pi=0;\
	char* l=NULL;	\
	theVals[0].kind=none;theVals[1].kind=none;\
	val.kind=none;\
	if (p==NULL) return NULL; \
	{A;theVals[0]=val;} \
	l=NULL;				\
	if (theVals[0].kind==none) { \
		*err = FIRST_ARG_IS_INVALID;\
		return NULL;\
	} \
	val.kind=none;\
	{B;theVals[1]=val;}	\
	if (theVals[1].kind==none) { \
		*err = SECOND_ARG_IS_INVALID;\
		return NULL;\
	} \
	VERIFY \
	code;		\
	HANDLE_ERROR \
	REMOVE_WARNS		\
	return p; \
}

#define ONE(A, code) { \
		Operand theVals[1],val; /*ONE*/	\
		Operand a[1];\
		int pi=0;\
		char* l=NULL;	\
		theVals[0].kind=none;\
		val.kind=none;\
		if (p==NULL) return NULL; \
		{A;theVals[0]=val;} \
		if (theVals[0].kind==none) { \
			*err = FIRST_ARG_IS_INVALID;\
			return NULL;\
		} \
		VERIFY \
		code;		\
		HANDLE_ERROR \
		REMOVE_WARNS		\
		return p; \
}

#define NONE(code) {if (p==NULL) return NULL; VERIFY; code; return p;}

#define MANY_NUMBERS(code) { \
	char* l; \
	int* nums;\
	int i,count=0;\
	if (p==((void *)0)) return ((void *)0);\
	l = p = strip(p," ");\
	if (oneOf(l, "\n\r")) {\
		*err = EMPTY_DATA;\
		return NULL;\
	}\
	do {\
		count++;\
		l = getInteger(strip(l," "), &i, err);\
		if (*err==NUMBER_ERR) {\
			*err = BAD_DATA;\
			return NULL;\
		}\
	} while (charIs(l, ','));\
	if (!oneOf(l, "\r\n")) {\
		*err = BAD_DATA;\
		return NULL;\
	}\
	nums = (int*)malloc(sizeof(int)*count);\
	i=0;\
	l=p;\
	while ((l=oneOf(getInteger(strip(l," "), &nums[i++], err), ",\n"))!=((void *)0));\
	code;\
	return *err!=0?NULL:p;\
}

#define STRING(code) { \
	char* text,*l; \
	if (p==NULL) return NULL; \
	p = strip(p," "); \
	l = charIs(allString(charIs(p, '"')), '"') ; \
	text = malloc((int)l-(int)p); \
	l = charIs(getAllString(charIs(p, '"'), text), '"') ; \
	code; \
	return p; \
}

#define GET_LINE_LABEL { \
	char* o=line; \
	if(!(isAlpha(line) && (l=charIs(getAllAlphasDigits(line, label), ':')))) { label[0]='\0'; l=o;} \
}

#define PARSE  { \
	int err=0,len; \
	Label label;\
	char*l;\
	lineCounter++;\
	GET_LINE_LABEL;\
	l=strip(l, " ");\
	len = strlen(line); \
	if (line[len-1]!='\n') {\
		line[len]='\n';\
		line[len+1]='\0';\
	}\
	if(oneOf(l, "\n\r;")) {}

#define TRY(cmd)      else if (cmd (matchWordD(l, #cmd) , &context, &err, label, lineCounter, line)) {}
#define TRY_DOT(cmd)  else if (cmd (matchWordD(charIs(l, '.'), or(charIs(#cmd,'_'),#cmd)), &context, &err, label,lineCounter, line)) {}
#define ELSE(msg)     else if (!err){ printf("Error at line %d '%s': %s\n",lineCounter,trimNewline(line),  msg);}\
					  else if (err) { \
						  dontGenerate = 1;\
						  switch(err) { \
						  	  case FIRST_ARG_IS_INVALID: printf(" Error at line %d '%s': first argument is not valid\n", lineCounter, trimNewline(line));break; \
						  	  case SECOND_ARG_IS_INVALID:printf(" Error at line %d '%s': second argument is not valid\n",lineCounter,  trimNewline(line));break; \
						  	  case COMMAND_NOT_TERMINATED:printf(" Error at line %d '%s': command has extra parameters\n", lineCounter,  trimNewline(line));break; \
						  	  case EMPTY_DATA:printf(" Error at line %d '%s': data is empty\n", lineCounter,  trimNewline(line));break; \
						  	  case BAD_DATA: printf(" Error at line %d '%s': data needs another number\n", lineCounter,  trimNewline(line));break; \
						  } \
					  }}

#define FOREACH_LINE(parse) while (fgets(line, MAXLINE-1, fr) != NULL) {parse;}

#define	FOREACH_FILE(baseAddress, doWithFile) \
	int i; \
	for (i = 1; i < argc; i++) { \
		int dontGenerate = 0;\
		char line[MAXLINE];\
		char file[20];\
		char objfile[20];\
		char entfile[20];\
		char extfile[20];\
		int lineCounter = 0;\
		Context context = { NULL, NULL, NULL, NULL, -1 };\
		FILE *fr, *fwo, *fwent, *fwex;\
		sprintf(file, "%s.as", argv[i]);\
		if ((fr = fopen(file, "r")) == NULL) {\
			printf("error opening %s file!!", file);\
		} else {\
			printf("Assembling file %s...\n", file);\
			doWithFile;\
			fclose(fr);\
			context.lastAsmOffset = computeAsmOffset(&context.codeList, baseAddress);\
			context.lastDataOffset = computeLabelOffset(&context.allLabels,context.lastAsmOffset);\
\
			if (execDeffered(&context.deferred) == 0) {\
				if (dontGenerate) {\
					printf("Errors found in file %s, skipping...\n\n", file);\
\
				} else {\
					printAsm(&context.codeList); \
					printf("\n"); \
					printData(&context.allLabels); \
					sprintf(objfile, "%s.obj", argv[i]);\
					if ((fwo = fopen(objfile, "w")) == NULL) {\
						printf("Error: cannot open '%s' file\n\n", objfile);\
					} else {\
						writeAsm(&context, fwo);\
						fclose(fwo);\
\
						sprintf(entfile, "%s.ent", argv[i]);\
						if ((fwent = fopen(entfile, "w")) == NULL) {\
							printf("Error: cannot open '%s' file\n\n", entfile);\
						} else {\
							extractEntries(&context.allLabels, fwent);\
							fclose(fwent);\
\
							if (context.externlabels != NULL) {\
								sprintf(extfile, "%s.ext", argv[i]);\
								if ((fwex = fopen(extfile, "w")) == NULL) {\
									printf("Error: cannot open '%s' file\n\n", entfile);\
								} else {\
									writeExEnt(&context.externlabels, fwex);\
									fclose(fwex);\
									printf("Finished Assembling file %s successfully.\n\n",file);\
								}\
							}\
						}\
					}\
\
				}\
			} else {\
				printf("Errors found in file %s, skipping...\n\n", file);\
			}\
			freeContext(&context); \
		}\
	}


void func(Operand oper1, Operand oper2, Label label);
void debugPrint(Operand oper);

#define GEN2(gen)		 gen##_gen(context, VAL(1), VAL(2), label, err, lineNumber,originalLine)
#define GEN1(gen) 		 gen##_gen(context, VAL(1), label, err,lineNumber,originalLine)
#define GEN0(gen) 		 gen##_gen(context, label, err, lineNumber,originalLine)
#define CALL_DATA(gen)   gen##_gen(context, label, err, nums, count, lineNumber,originalLine)
#define CALL_STRING(gen) gen##_gen(context, label, err, text, lineNumber,originalLine)

#define MOV(OP1, OP2) OPER2(0,OP1,OP2)
#define CMP(OP1, OP2) OPER2(1,OP1,OP2)
#define ADD(OP1, OP2) OPER2(2,OP1,OP2)
#define SUB(OP1, OP2) OPER2(3,OP1,OP2)
#define LEA(OP1, OP2) OPER2(6,OP1,OP2)

#define NOT(OP1) OPER1(4,OP1)
#define CLR(OP1) OPER1(5,OP1)
#define INC(OP1) OPER1(7,OP1)
#define DEC(OP1) OPER1(8,OP1)
#define JMP(OP1) OPER1(9,OP1)
#define BNE(OP1) OPER1(10,OP1)
#define RED(OP1) OPER1(11,OP1)
#define PRN(OP1) OPER1(12,OP1)
#define JSR(OP1) OPER1(13,OP1)

#define RTS  OPER0(14)
#define STOP OPER0(15)

#define CMD(name) char* name(char* p, Context* context, int* err, char* label, int lineNumber, char* originalLine)

CMD(data);

CMD(cmp);
CMD(mov);
CMD(add);
CMD(sub);
CMD(lea);

CMD(prn);
CMD(not);
CMD(clr);
CMD(inc);
CMD(dec);
CMD(jmp);
CMD(bne);
CMD(red);
CMD(jsr);
CMD(rts);
CMD(stop);

CMD(_extern);
CMD(entry);
CMD(data);
CMD(string);

#endif /* ASMDSL_H_ */
