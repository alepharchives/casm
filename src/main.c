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

#include "lists.h"
#include "DoubleLinkedList.h"

#include "asmdsl.h"
#include "gendsl.h"
#include "parser.h"

int main(void) {
	char line[1000];
	node* n;
	byte b;
	int lineCounter = 0;
	list codeList = NULL;
	list allLabels  = NULL;
	list deferred = NULL;
	list dataList = NULL;
	Context context = {NULL, NULL, NULL, NULL};\


	/* mov label[%ass], r3*/
	Operand operand1, operand2;
	operand1.kind = label_with_index;
//	operand1.get.oneIndex = 4;
	operand2.kind = direct;
	strcpy(operand2.get.direct, "label");

	//MOV(REGISTER(source), DIRECT_LABEL(dest))






		/* MOV #45, r2 */
	/*operand1.get.constant = 45;*/
/*	allLabels = append(allLabels, newLabel("ass", 40));
	{
		Operand operand1, operand2;
		operand1.kind = reg;
		operand1.get.reg = 4;
		operand2.kind = direct;
		strcpy(operand2.get.direct, "label");

		MOV(REGISTER(source), DIRECT_LABEL(dest))

	}*/

		/* mov label, ass
			MOV(REGISTER(source), DIRECT_LABEL(dest))
		 * */

/* mov label[%ass], r3
   MOV(LABEL1D(source), REGISTER(dest));
*/

/*	{
			Operand operand1, operand2;
			operand1.kind = label_with_index;
			strcpy(operand1.get.oneIndex.label, "label"); /* resolve offset*/
			/*strcpy(operand1.get.oneIndex.index, "ass"); /* has to defer a call to resolve distance*/
			/*operand1.get.reg = 4;
			operand2.kind = direct;
			strcpy(operand2.get.direct, "label");
	}*/

/* these two has to work!

    MOV(LABEL2D(source), REGISTER(dest));
*/




/*
		MOV(DIRECT_LABEL(source), DIRECT_LABEL(dest))
	MOV(CONSTANT(source), DIRECT_LABEL(dest))
	MOV(CONSTANT(source), CONSTANT(dest))
	allLabels = append(allLabels, newLabel("label", 50));

	INVOKE(deferred);
*/


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

		 TRY_DOT(_extern)
		 TRY_DOT(entry)
		 TRY_DOT(data)
		 TRY_DOT(string)
		ELSE("no such command")
		fflush(stdout);
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
