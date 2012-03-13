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

typedef char byte;

typedef struct {
	byte op_code, extra1, extra2;
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
	ASM(n)->extra1=0;
	ASM(n)->extra2=0;
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

int main(void) {
	char line[1000];
	node* n;
	list l = NULL;
	byte b;
	char* r="ass";

	l = append(l, newDeferedNode(deferLabelAddrResolution,&b,r));
	l = append(l, newDeferedNode(deferLabelPositionResolution,&b,r));
	INVOKE(l);
	l=l->next;
	INVOKE(l);

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
