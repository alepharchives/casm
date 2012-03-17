/*
 * asmDsl.c
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */


#include "asmdsl.h"

void cmp_func(Operand oper1, Operand oper2, Label label) {
	printf("CMP\n");
	if (label[0]!='\0')	printf("label %s\n", label);
	debugPrint(oper1);
	debugPrint(oper2);
}

void mov_func(Operand oper1, Operand oper2, Label label) {
	printf("MOV\n");
	if (label[0]!='\0')	printf("label %s\n", label);
	debugPrint(oper1);
	debugPrint(oper2);
}

void add_func(Operand oper1, Operand oper2, Label label) {
	printf("ADD\n");
	if (label[0]!='\0')	printf("label %s\n", label);
	debugPrint(oper1);
	debugPrint(oper2);
}
void debugPrint(Operand oper) {
	switch (oper.kind) {
		case reg:
			printf("got register %i\n", oper.get.reg);
			break;
		case constant:
			printf("got constant %i\n", oper.get.constant);
			break;
		case direct:
			printf("got direct %s\n", oper.get.direct);
			break;
		case label_with_index:
			printf("got label with index %s %s\n", oper.get.oneIndex.label, oper.get.oneIndex.index);
			break;
		case label_with_two_indices:
			printf("got label with two index %s %s %d\n", oper.get.twoIndice.label, oper.get.twoIndice.index, oper.get.twoIndice.reg);
			break;
		default:
			printf("error!");
			break;
	}
}

/* code in the table on page 33 */
CMD(cmp) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR5(REG, DIRECT, LABEL1, LABEL2, CONST),  cmp_func(VAL(1), VAL(2), label))




CMD(mov) /* TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		mov_func(VAL(1), VAL(2), label)) */
{ \
	Operand theVals[2],val; /*TWO*/	\
	Operand a[2];\
	int pi=0;\
	char* l=NULL;	\
	if (p==NULL) return NULL; \
	{int count=5;		 \
		Operand a[5] ;		    \
		int pi=0,i;			\
		Operand val;			\
		for (i=0;i<count;i++) { a[i].kind=none;}		        \
    l = NULL;			\
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
	pi++;;if (l==NULL) /*CONST*/ { \
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
    pi++;;if (l==NULL) /*DIRECT*/ { \
		l = strip(getAllAlphas(strip(p," "), a[pi].get.direct), " "); \
		l = oneOf(l,",\n"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = direct; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;;if (l==NULL) /*LABEL1*/ { \
		l = charIs(getAllAlphasDigits(charIs(charIs(getAllAlphas(strip(p," "), a[pi].get.oneIndex.label), '[') ,'%'), a[pi].get.oneIndex.index), ']'); \
		l = oneOf(l,",\n"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = label_with_index; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;;if (l==NULL) /*LABEL2*/ { \
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
	pi++;;			\
	l = NULL;;theVals[0]=val;} \
	l=NULL;				\
	if (theVals[0].kind==none) return NULL;\
	{int count=4;		 \
		Operand a[4] ;		    \
		int pi=0,i;			\
		Operand val;			\
		for (i=0;i<count;i++) { a[i].kind=none;}		        \
    l = NULL;			\
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
	pi++;;if (l==NULL) /*DIRECT*/ { \
		l = strip(getAllAlphas(strip(p," "), a[pi].get.direct), " "); \
		l = oneOf(l,",\n"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = direct; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;;if (l==NULL) /*LABEL1*/ { \
		l = charIs(getAllAlphasDigits(charIs(charIs(getAllAlphas(strip(p," "), a[pi].get.oneIndex.label), '[') ,'%'), a[pi].get.oneIndex.index), ']'); \
		l = oneOf(l,",\n"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = label_with_index; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;;if (l==NULL) /*LABEL2*/ { \
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
	pi++;;			\
	l = NULL;;theVals[1]=val;}	\
	if (theVals[1].kind==none) return NULL;\
	mov_func(theVals[1-1], theVals[2-1], label);		\
	pi=val.get.constant;pi++;pi=a[0].get.constant;		\
	return p; \
}




CMD(add) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		add_func(VAL(1), VAL(2), label))
CMD(sub) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		add_func(VAL(1), VAL(2), label))
CMD(lea) TWO(OR3(DIRECT, LABEL1, LABEL2),             OR4(REG, DIRECT, LABEL1, LABEL2), 		add_func(VAL(1), VAL(2), label))

CMD(prm) ONE(OR5(REG, DIRECT, CONST,  LABEL1, LABEL2),		debugPrint(VAL(1)))
CMD(not) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(clr) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(inc) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(dec) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(jmp) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(bne) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(red) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(jsr) ONE(DIRECT,										debugPrint(VAL(1)))

CMD(rts)  NONE(printf("rts!"))
CMD(stop) NONE(printf("stop!"))

