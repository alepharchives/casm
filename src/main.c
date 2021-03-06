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

#include "commons.h"
#include "lists.h"
#include "DoubleLinkedList.h"

#include "asmdsl.h"
#include "gendsl.h"
#include "parser.h"

int main(int argc, char *argv[]) {
	int baseAddress = 100;

	FOREACH_FILE(
		FOREACH_LINE(
			PARSE
				TRY(cmp)
				TRY(mov)
				TRY(add)
				TRY(sub)
				TRY(lea)
				TRY(prn)
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
			ELSE("no such command")))

	return 0;
}
