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
	none = 0,
	constant,
	direct,
	label_with_index,
	label_with_two_indices,
	reg
} addressingE;

typedef char* label;

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

#define INDEX(v, code) {\
	int v;\
	\
}

#define LABEL2D(index, label, reg, code) { \
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

void printMe2(addressingE kind, int v) {
	switch (kind) {
		case reg:
			printf("got register %i\n", v);
			break;
		case constant:
			printf("got constant %i\n", v);
			break;
		default:
			printf("Shouldn't get here, use ERR!");
			break;
	}
}
#define DO_(c) \
		int count=c;		 \
		addressingE kind=none, kinds[c];\
		int a[c] ;		    \
		int pi;			\
		int val=-1;			\
		for (pi=0;pi<count;pi++) { a[pi]=-1;kinds[pi]=none; } \
		pi=0;

#define FIN_OR(code) \
		for (pi=0;val==-1&&pi<count;pi++) {val=a[pi];kind=kinds[pi];} \
		if (kind != none) { \
			code;			\
		};

#define FIN_AND(code) \
		code;

#define AND2(A,B,code) { \
	int ai = 0; \
	addressingE theKinds[2]; \
	int theVals[2]		   ; \
	DO_(2)				\
	A;l=NULL;ai++;B;				\
	FIN_AND(code)		\
}

#define AND3(A,B,code) { \
	DO_(3)				\
	A;B;				\
	FIN_AND(code)		\
}

#define OR2(A,B,code) { \
	DO_(2)				\
	A;B;				\
	FIN_OR(code)		\
}

#define OR3(A,B,C,code) { \
	DO_(3)			\
	A;B;C;				\
	FIN_OR(code)		\
}

#define OR4(A,B,C,D,code) { \
	DO_(4)			\
	A;B;C;D;			\
	FIN_OR(code)		\
}

#define KIND kind
#define VAL val
#define GET_VAL(i) theVals[i-1]
#define GET_KIND(i) theKinds[i-1]
#define _(i)	a[i-1]
#define _K(i)	kinds[i-1]

#define REG2 \
	if (l==NULL) { \
		char* n; \
		l = charIs(strip(p," "), 'r'); \
			if (l!=NULL) { \
				if (n=oneOf(strip(isDigit(l), " "),",\n")) { \
					a[pi] = 0; \
					a[pi] = (char)*(l)-'0'; \
					kinds[pi] = reg;\
				} \
				else l = NULL; \
			if (l!=NULL){ \
				p=n;\
			}\
		}\
	}\
	pi++; \


#define CONST2  \
	if (l==NULL) { \
		l = charIs(strip(p," "),'#');\
		if (l!=NULL) { \
			l = oneOf(getInteger(strip(l," "),&a[pi], &err), ",\n"); \
			if (l!=NULL) { \
				kinds[pi] = constant;\
				p=l; \
			} \
		}\
	}\
    pi++;\

#define NOP theVals[ai]=val;theKinds[ai]=kind;

/*for (pi=0;pi<count;pi++) {theVals[pi]=a[pi];theKinds[pi]=kinds[pi];}
*/

void func(addressingE kind1, int v1, addressingE kind2, int v2) {
	printMe2(kind1, v1);
	printMe2(kind2, v2);
}

int main(void) {

	char line[1000];
	int err = 0;
	int f = 0;
	char *l;
	char *p;
	node* n;
	list labels = NULL;
	/*
	n = malloc(sizeof(node));
	n->data = (char*)malloc(10);
	strcpy(n->data, "ass");

	labels = insertHead(labels, n);
	n = malloc(sizeof(node));
	n->data = (char*)malloc(10);
	strcpy(n->data, "123123");
	labels = insertHead(labels, n);

	printBackwards(labels);

	 endlessly read lines, passing them to our dsl.
	 * since the code is so declarative, i see no reason to comment this file any more :) */
	while (1) {
		getline(line, sizeof(line));
		p = line;
		l=NULL;

		/* first attempt.. */
		/* REG(DO, r1, CONST(OR, c1, ERR(OR, "bad input\n", printMe(r1,c1)))); */

		/* new attempt.. */
		/* get one of two values */
		printf("get first one: ");
		OR2(REG2, CONST2, printMe(_(1), _(2)));

		p = line;
		l=NULL;
		/* get two values, each one could be one of two values
		 */

		printf("full get:");
		AND2(OR2(REG2, CONST2, NOP),OR2(REG2, CONST2, NOP), func(GET_KIND(1), GET_VAL(1), GET_KIND(2), GET_VAL(2)));

		p = line;
		l=NULL;

		printf("first is reg:");
		AND2(REG2 ,OR2(REG2, CONST2, NOP), func(_K(1), _(1), GET_KIND(2), GET_VAL(2)));


		/*
		LABEL(INDEX(pre), l, INDEX(post))
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

void v()	 {
	char line[1000];
	int err = 0;
	int f = 0;
	char *l;
	char *p;
	node* n;
	list labels = NULL;

	{
		int count=2;
			addressingE kind=none, kinds[2];
			int a[2] ;
			int pi;
			int val=-1;
			for (pi=0,a[0]=-1,kinds[0]=none;pi<count;pi++,a[pi]=-1,kinds[pi]=none);
		{
		int count=3;
			addressingE kind=none, kinds[3];
			int a[3] ;
			int pi;
			int val=-1;
			for (pi=0,a[0]=-1,kinds[0]=none;pi<count;pi++,a[pi]=-1,kinds[pi]=none);
		if (l==((void *)0)) {
			l = charIs(strip(p," "), 'r');
				if (l!=((void *)0)) {
					if (isDigit(l)) {
						a[pi] = 0;
						a[pi] = (char)*(l)-'0';
						kinds[pi] = reg;
					}
					else l = ((void *)0);
				if (l!=((void *)0)){
					l++;
					p=l;
				}
			}
		}
		pi++;;if (l==((void *)0)) {
			l = charIs(strip(p," "),'#');
			if (l!=((void *)0)) {
				l = getInteger(strip(l," "),&a[pi], &err);
				if (l!=((void *)0)) {
					kinds[pi] = constant;
					l++;
					p=l;
				}
			}
		}
	    pi++;
	;if (l==((void *)0)) {
			l = charIs(strip(p," "),'#');
			if (l!=((void *)0)) {
				l = getInteger(strip(l," "),&a[pi], &err);
				if (l!=((void *)0)) {
					kinds[pi] = constant;
					l++;
					p=l;
				}
			}
		}
	    pi++;
	;
		if (kind != none) {
				for (pi=0,val=a[0],kind=kinds[0];val==-1&&pi<count;pi++,val=a[pi],kind=kinds[pi]);	;;;
			};
	};{
		int count=2;
			addressingE kind=none, kinds[2];
			int a[2] ;
			int pi;
			int val=-1;
			for (pi=0,a[0]=-1,kinds[0]=none;pi<count;pi++,a[pi]=-1,kinds[pi]=none);
		if (l==((void *)0)) {
			l = charIs(strip(p," "), 'r');
				if (l!=((void *)0)) {
					if (isDigit(l)) {
						a[pi] = 0;
						a[pi] = (char)*(l)-'0';
						kinds[pi] = reg;
					}
					else l = ((void *)0);
				if (l!=((void *)0)){
					l++;
					p=l;
				}
			}
		}
		pi++;;if (l==((void *)0)) {
			l = charIs(strip(p," "),'#');
			if (l!=((void *)0)) {
				l = getInteger(strip(l," "),&a[pi], &err);
				if (l!=((void *)0)) {
					kinds[pi] = constant;
					l++;
					p=l;
				}
			}
		}
	    pi++;
	;
		if (kind != none) {
				for (pi=0,val=a[0],kind=kinds[0];val==-1&&pi<count;pi++,val=a[pi],kind=kinds[pi]);	;;;
			};
	};
		if (kind != none) {
				func(kinds[1-1], a[1-1], kinds[2-1], a[2-1]);
			};
	}
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
