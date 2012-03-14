/*
 * main.c
 *
 *  Created on: Dec 26, 2011
 *      Author: shlomi vaknin
 *
 *  maman 22
 *
 * In this project i created a small scale DSL (domain specific language) that lets us
 * easily describe interactive commands within our domain (namely matrix manipulation).
 *
 * parser.c and parser.h contains the code that parses a given line of user input.
 * it is heavily influenced by parser combinators. Since C cannot use lambda
 * expressions, the implementation is not really functional in style. more details on
 * the technique i used is found in these files.
 *
 * matdsl.c and matdsl.h contains the definitions of all dsl's directives and commands.
 * these were designed to be as composable as possible under the restriction of the C language.
 * To achieve that i had to slightly abuse the #define keyword. again, further details
 * are given in these files. (the real magic happens in matdsl.c!)
 *
 * main.c file contains the simple logic for our program - read one line at a time,
 * and try out all of our commands on that line. if all fails, an error is printed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asmdsl.h"
#include "parser.h"
#include "DoubleLinkedList.h"

/* define our global matrices
Mat Mats[MAX_MATS];
*/

typedef unsigned char byte;

typedef struct {
	int op_code, word1, word2;
	byte size;
	byte offset;
} asm_node ;

typedef struct {
	void (*f)(byte*, char*);
	byte* into;
	char* label;
}defered_node;

#define ASM(n) ((asm_node*)n->data)
#define INVOKE(n) ((defered_node*)n->data)->f((((defered_node*)n->data)->into), (((defered_node*)n->data)->label))
node* newAsmNode() {
	node* n = malloc(sizeof(node));
	n->prev=NULL;
	n->data=malloc(sizeof(asm_node));
	ASM(n)->op_code=0;
	ASM(n)->word1=0;
	ASM(n)->word2=0;
	ASM(n)->size=0;
	ASM(n)->offset=0;
	return n;
}

node* newDeferedNode(void (*f)(byte*, char*), byte* into, char* label) {
	node* n = malloc(sizeof(node));
	defered_node* d = malloc(sizeof(defered_node));
	d->f = f;
	d->into=into;
	d->label=label;
	n->prev=NULL;
	n->data=d;
	return n;
}

void deferLabelAddrResolution(byte* into, char* label) {
	*into = 3;
}

void deferLabelPositionResolution(byte* into, char* label) {
	*into = 5;
}

void (*f(byte*, char*));
typedef struct Foo {
    int flag    : 1;
    int counter : 15;
} Foo;
typedef
	union {
	int code;
	struct {
		int destReg:3;
		int destKind:3;
		int sourceReg:3;
		int sourceKind:3;
		int op:4;
	}bit;
} OpCode ;

#define REGISTER(T) \
	code.bit.T ## Kind = theOperand->kind; /* REGISTER */ \
	code.bit.T ## Reg  = theOperand->get.reg;

#define CONSTANT(T)  \
	code.bit.T ## Kind = theOperand->kind; /* CONSTANT */\
	code.bit.T ## Reg  = 0;\
	*theWord=theOperand->get.constant; \
	size++;theWord++; \

#define MOV(OP1, OP2) { \
	OpCode code;\
	int words[2]={0,0}, *theWord=words;\
	byte size=1; \
	code.bit.op=0;\
	Operand* theOperand=&operand1;\
	code.code=0;\
	OP1; \
	theOperand=&operand2;\
	OP2; \
	\
	node* n = newAsmNode(); \
	codeList = append(codeList, n);\
	ASM(n)->op_code = code.code; \
	ASM(n)->word1 = words[0];\
	ASM(n)->word2 = words[1];\
	ASM(n)->size=size; \
}

int main(void) {
	char line[1000];
	node* n;
	list codeList = NULL;
	byte b;
/*	char* r="ass";

	l = append(l, newDeferedNode(deferLabelAddrResolution,&b,r));
	l = append(l, newDeferedNode(deferLabelPositionResolution,&b,r));
	INVOKE(l);
	l=l->next;
	INVOKE(l);
*/


	/* MOV #45, r2 */
	Operand operand1, operand2;
	operand1.kind = constant;
	operand1.get.constant = 45;
	operand2.kind = reg;
	operand2.get.reg = 2;

	MOV(CONSTANT(source), REGISTER(dest))

	MOV(CONSTANT(source), CONSTANT(dest))
/*
	{
		OpCode code={0,0,0,0,0};
		byte word1;
		printf("%d\n", sizeof(code));
		fflush(stdout);
		code.op=0;
		code.destReg=operand2.get.reg;
		code.destKind = operand2.kind;

		code.sourceKind=operand1.kind;
		code.sourceReg=0;
		word1 = operand1.get.constant;

		printf("%d\n", code);
		fflush(stdout);
	}

*/
/*
	l = append(l,newDeferedNode(deferLabelAddrResolution()));
	n = newAsmNode();
	ASM(n)->extra1 = 3;
	ASM(n)->op_code = 10;
	l = append(l, n);

	n = newAsmNode();
	ASM(n)->extra1 = 4;
	l = append(l, n);

*/

	/*
	 endlessly read lines, passing them to our dsl.
	 * since the code is so declarative, i see no reason to comment this file any more :) */
	while (1) {
		getline(line, sizeof(line));

		PARSE(line)
		 TRY(cmp)
		 TRY(mov)
		 TRY(add)
		 TRY(sub)
		 TRY(lea)
		 TRY(prm)
		 TRY(not)
		 TRY(clr)
		 TRY(inc)
		 TRY(dec)
		 TRY(jmp)
		 TRY(bne)
		 TRY(red)
		 TRY(jsr)
		 TRY(rts)
		 TRY(stop)
		ELSE("no such command")
	}

	return 0;
}

/*
node* n;

int i = 42;

if (i-=2) {
	printf("ass\n");
}

printf("%d\n", (i+=12)-(i+=67));


n = malloc(sizeof(node));
n->data = 4;
insertSorted(n);

n = malloc(sizeof(node));
n->data = 5;
insertSorted(n);

n = malloc(sizeof(node));
n->data = 2;
insertSorted(n);

n = malloc(sizeof(node));
n->data = 9;
insertSorted(n);

print();
printBackwards();

n = malloc(sizeof(node));
n->data = 6;
insertSorted(n);

n = malloc(sizeof(node));
n->data = 1;
insertSorted(n);

n = malloc(sizeof(node));
n->data = 16;
insertSorted(n);

print();
printBackwards();

freeList();
*/
