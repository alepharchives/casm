/*
 * asdDsl.h
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */

#ifndef ASMDSL_H_
#define ASMDSL_H_

#include "parser.h"

typedef enum {
	none = 0,
	constant,
	direct,
	label_with_index,
	label_with_two_indices,
	reg
} addr;

#define MAX_LABEL 30

typedef int Reg;
typedef int Constant;
typedef char Label[MAX_LABEL];
typedef Label Direct;
typedef struct  {
		char label[MAX_LABEL];
		char index[MAX_LABEL];
	} LabelOneIndex;
typedef struct  {
		char index[MAX_LABEL];
		char label[MAX_LABEL];
		int reg;
	} LabelTwoIndice;

typedef struct {
	addr kind;
	union {
		Reg reg;
		Constant constant;
		Direct direct;
		LabelOneIndex oneIndex;
		LabelTwoIndice twoIndice;
	} get;
} Operand;

#define DO_(c) \
		int count=c;		 \
		Operand a[c] ;		    \
		int pi=0,i;			\
		Operand val;			\
		for (i=0;i<count;i++) { a[i].kind=none;}

#define FIN_OR(code) \
		for (i=0;val.kind==none&&i<count;i++) {val=a[i];} \
		if (val.kind != none) { \
			code;			\
		};

#define FIN_AND(code) \
		code;

#define REMOVE_WARNS pi=val.get.constant;pi++;pi=a[0].get.constant; /* a silly line just to remove annoying "unused" warnings*/

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
		l = strip(getAllAlphas(strip(p," "), a[pi].get.direct), " "); \
		l = oneOf(l,",\n"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = direct; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;

#define LABEL1 \
	if (l==NULL) /*LABEL1*/ { \
		l = charIs(getAllAlphasDigits(charIs(charIs(getAllAlphas(strip(p," "), a[pi].get.oneIndex.label), '[') ,'%'), a[pi].get.oneIndex.index), ']'); \
		l = oneOf(l,",\n"); \
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
		l = oneOf(l,",\n"); \
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
				if ((n=oneOf(strip(isDigit(l), " "),",\n"))) { \
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
			l = oneOf(getInteger(strip(l," "),&a[pi].get.constant, err), ",\n"); \
			if (l!=NULL) { \
				a[pi].kind = constant; \
				val = a[pi];\
				p=l; \
			} \
		}\
	}\
    pi++;

#define TWO(A,B,code) { \
	Operand theVals[2],val; /*TWO*/	\
	Operand a[2];\
	int pi=0;\
	char* l=NULL;	\
	{A;theVals[0]=val;} \
	l=NULL;				\
	if (theVals[0].kind==none) return NULL;\
	{B;theVals[1]=val;}	\
	if (theVals[1].kind==none) return NULL;\
	FIN_AND(code)		\
	REMOVE_WARNS		\
	return p; \
}

#define ONE(A, code) { \
		Operand theVals[2],val; /*ONE*/ \
		Operand a[2];\
		int pi=0;\
		char* l=NULL;	\
		{A;theVals[0]=val;} \
		FIN_AND(code)		\
		REMOVE_WARNS		\
		return p; \
}

#define GET_LINE_LABEL { \
	char* o=line; \
	if(!(isAlpha(line) && (l=charIs(getAllAlphasDigits(line, label), ':')))) { label[0]='\0'; l=o;} \
}

#define PARSE(line)  {int err=0;Label label;char*l;GET_LINE_LABEL;l=strip(l, " ");if(0) {}
#define TRY(cmd)      else if (cmd (matchWordD(l, #cmd) , &err, label)) {}
#define ELSE(msg)     else if (!err){ printf("%s\n",msg);}}

void func(Operand oper1, Operand oper2, Label label);
void debugPrint(Operand oper);

#define CMD(name) char* name(char* p, int* err, char* label)

CMD(mov);
#endif /* ASMDSL_H_ */
