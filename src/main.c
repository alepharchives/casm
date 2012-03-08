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

int main(void) {

	char line[1000];
	int err = 0;
	int f = 0;
	char *l;

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
	/* endlessly read lines, passing them to our dsl.
	 * since the code is so declarative, i see no reason to comment this file any more :) */
	while (1) {
		getline(line, sizeof(line));
		l = line;
		{
			char *p = l;
			{ /* register */
				char r=-1;
				l = charIs(strip(p," "), 'r');
				if (l!=NULL) {
					if (isDigit(l)){
						r = (char)*l;
					}
					if (l!=NULL){
						l++;
						p=l;
					}
				}
			}
			if (l==NULL) {
				int i=-1; /*constant*/
				l = charIs(strip(p," "),'#');
				if (l!=NULL) {
					l = getInteger(strip(l," "),&i, &err);
					if (l!=NULL){
						l++;
						p=l;
					}
				}
			}
			if (l==NULL) { /*label*/

			}


			printf("%d\n", f);
		}
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
