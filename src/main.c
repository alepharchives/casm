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

#include "mat.h"
#include "matdsl.h"
#include "parser.h"
#include "DoubleLinkedList.h"

/* define our global matrices */
Mat Mats[MAX_MATS];

typedef enum {
	immediate = 0,
	direct,
	label_with_index,
	label_with_two_indices,
	reg
} addressingE;

typedef struct {
	addressingE kind;
	void *d1,*d2,*d3; /* data holders according with kind */
} addressing;

#define TWO (A,B,code ){ \
	addressing op1; \
	addressing op2; \
					\
	A(&op1);		\
	B(&p2);			\
	code;			\
}

#define ERR( B , S ,code) { \
	if (B) { \
		printf("%s", S);\
	} \
	else \
		code; \
}

#define OR l==NULL
#define DO 1

#define REG( B , r ,code) { \
	 /* register */ \
	int r = -1; \
	if (B) { \
		l = charIs(strip(p," "), 'r'); \
			if (l!=NULL) { \
				if (isDigit(l)) { \
					r = 0; \
					r = (char)*(l)-'0'; \
				} \
				else l = NULL; \
			if (l!=NULL){ \
				l++;\
				p=l;\
			}\
		}\
	}\
	code; \
}

#define CONST(B, i, code) { \
	int i = -1; \
	if (B) { \
		l = charIs(strip(p," "),'#');\
		if (l!=NULL) { \
			l = getInteger(strip(l," "),&i, &err); \
			if (l!=NULL) { \
				l++; \
				p=l; \
			} \
		}\
	}\
	code;\
}


void printMe(int r, int i) {
	if (r!=-1) {
		printf("got register %i\n", r);
		return;
	}
	if (i!=-1) {
		printf("got constant %d\n", i);
		return;
	}
	printf("Shouldn't get here, use ERR!");
}

int main(void) {

	char line[1000];
	int err = 0;
	int f = 0;
	char *l;
	char *p;

	/* endlessly read lines, passing them to our dsl.
	 * since the code is so declarative, i see no reason to comment this file any more :) */
	while (1) {
		getline(line, sizeof(line));
		p = line;
		l=NULL;

		REG(DO, r1, CONST(OR, c1, ERR(OR, "bad input\n", printMe(r1,c1))));

		/*
		 PARSE(line)
		 TRY(read_mat)
		 TRY(add_mat)
		 TRY(sub_mat)
		 TRY(mul_mat)
		 TRY(mul_scalar)
		 TRY(trans_mat)
		 TRY(print_mat)
		 TRY(stop)
		 ELSE("no such command")
		 */
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
