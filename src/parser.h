/*
 * parser.h
 *
 *  Created on: Dec 22, 2011
 *      Author: shlomi vaknin
 *
 *  This file contains the implementation of our parser-combinator-like parser utility.
 *  since C is not functional at all, we couldnt really use combinators techniques, but
 *  i still tried to keep the best feature of parser-combinators, which is composability.
 *
 *  the first argument of each parser function is the head of the string to parse,
 *  the return value of each parser function is NULL on failure, or the head of the REMAINING string,
 *  that still needs to be parsed. meaning each parser CONSUMES as much input as it can, and
 *  returns the rest of the input.
 */

#ifndef PARSER_H_
#define PARSER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* this is used to by the parser error mechanism */
#define NO_ERR 0
#define CHARIS_ERR 1
#define NUMBER_ERR 2

/* reads a line of input from the keyboard into line, a maximum of max characters long.
 * The return value is the length of the line read. */
int getline(char *line, int max);

/* Parser Functions
 *------------------
 * each parser function receives the head of the input stream in p, and returns the head of
 * the unconsumed string.
 */

/* charIs - succeeds iff the head of the input string is c. */
char* charIs(char* p, char c);

/* same as charIs, but has also an error indicator */
char* charIsE(char* p, char c, int* err);

/* matchWord succeeds iff the input string contains exactly the string m */
char* matchWord(char* p, char* m);

/* same as matchWord, but demands that after the matched string there will be a whitespace */
char* matchWordD(char* p, char* m);

/* isDigit succeeds iff head of p is a digit */
char* isDigit(char* p);

/* this parser consumes the input string, as long as the head is a digits.
 * it returns the rest of the string, where the head is certain to be a non-digit char */
char* allDigits(char *p);

/* a parser that is composed of the previous parsers to get a number out of the input string.
 * it succeeds iff the input line contains a floating point number. if successful, the result
 * is given in res, if fails, the error is given in error and return value is NULL.*/
char* getNumber(char*p, float* res, int* err);
char* getInteger(char*p, int* res, int* err);

/* a special helper function, that returns true if character c is contained by string p */
int contains (char c, char* p);

/* a parser that uses contains above to match the head of the string with one of the letters
 * in string c */
char* oneOf(char* p, char* c);

/* strip - this parser always succeeds. it uses contains above to strip all leading
 * chars from the input string p, that are contained in string c */
char* strip(char* p,char* c);

/* a parser that succeeds iff either parsers that were given to it as parameters succeeded */
char* or (char* p1, char* p2);

/* get the next digit */
char* getDigit(char* p, char* d);

/* consume all alpha characters */
char* allAlphas(char *p);

/* isAlpha succeeds iff head of p is a an alpha character */
char* isAlpha(char* p);

/* consume all alphas and get them too */
char* getAllAlphas(char *p, char* out);

/* consume all alpha and digits, and get back the result.
 * BUT! this function makes sure that the first character is an alpha and NOT a digit! */
char* getAllAlphasDigits(char *p, char* out);

/* consume all alphas or digits */
char* allAlphasOrDigit(char *p);

/* consume all printable characters, and get the consumed part */
char* getAllString(char *p, char* out);

/* consume all printable characters */
char* allString(char *p);

/* checks if the next character is printable */
char* isStringChar(char* p);

#endif /* PARSER_H_ */
