/*
 * parser.h
 *
 *  Created on: Dec 22, 2011
 *      Author: shlomi vaknin
 *
 *  this is the implementation of the pseudo-parser-combinator style parser.
 */

#include "parser.h"

/* reads a line of input from the keyboard into line, a maximum of max characters long.
 * The return value is the length of the line read. */
int getline(char *line, int max)
{
   if (fgets(line, max, stdin) == NULL)
	   return 0;
   else
	   return strlen(line);
}

/* same as charIs, but has also an error indicator */
char* charIsE(char* p, char c, int* err) {
	*err=0;
	if (p!=NULL && *p!='\0'&& *p==c) return ++p;
	*err=CHARIS_ERR;
	return NULL;
}

/* a parser that uses contains to match the head of the string with one of the letters
 * in string c */
char* oneOf(char* p, char* c) {
	if (p!=NULL && *p!='\0'&& contains(*p,c)) return ++p;
	return NULL;
}

/* charIs - succeeds iff the head of the input string is c. */
char* charIs(char* p, char c) {
	if (p!=NULL && *p!='\0'&& *p==c) return ++p;
	return NULL;
}

int contains (char c, char* p) {
	if (*p=='\0') return 0;
	if (*p==c) return 1;
	return contains(c, p+1);
}

/* strip - this parser always succeeds. it uses contains above to strip all leading
 * chars from the input string p, that are contained in string c */
char* strip(char* p,char* c) {
	while (p != NULL && *p!='\0' && contains(*p, c)) p++;
	return p;
}

/* a parser that succeeds iff either parsers that were given to it as parameters succeeded */
char* or (char* p1, char* p2) {
	if (p1 == NULL) return p2;
	return p1;
}

/* isDigit succeeds iff head of p is a digit */
char* isDigit(char* p) {
	if (*p == '\0') return NULL;
	if (*p>='0'&&*p<='9') return p+1;
	return NULL;
}

/* get that digit */
char* getDigit(char* p, char* d) {
	if (p==NULL || *p == '\0') return NULL;
	if (*p>='0'&&*p<='9') {
		*d = *p;
		return p+1;
	}
	return NULL;
}

/* isAlpha succeeds iff head of p is a an alpha character */
char* isAlpha(char* p) {
	if (*p == '\0') return NULL;
	if ((*p>='a'&&*p<='z') || (*p>='A'&&*p<='Z') ) return p+1;
	return NULL;
}

/* consume all alpha characters */
char* allAlphas(char *p) {
	if (p==NULL || *p=='\0') return NULL;
	if (!isAlpha(p)) return p;
	return allAlphas(p+1);
}

/* consume all alphas or digits */
char* allAlphasOrDigit(char *p) {
	if (p==NULL || *p=='\0') return NULL;
	if (!(isAlpha(p) || isDigit(p))) return p;
	return allAlphasOrDigit(p+1);
}

/* consume all alphas and get them too */
char* getAllAlphas(char *p, char* out) {
	char *e = p, o;
	if (p==NULL || *p=='\0') return NULL;
	p = allAlphas(p);
	if (p==NULL || *p=='\0') return NULL;
	o = *p;
	*p = '\0';
	strcpy(out, e);
	*p = o;
	return p;
}

/* consume all alpha and digits, and get back the result.
 * BUT! this function makes sure that the first character is an alpha and NOT a digit! */
char* getAllAlphasDigits(char *p, char* out) {
	char *e = p, o;
	if (p==NULL || *p=='\0') return NULL;
	p = isAlpha(p);
	p = allAlphasOrDigit(p);
	if (p==NULL || *p=='\0') return NULL;
	o = *p;
	*p = '\0';
	strcpy(out, e);
	*p = o;
	return p;
}

/* checks if the next character is printable */
char* isStringChar(char* p) {
	if (*p == '\0' || *p=='"') return NULL;
	if ((isprint(*p)) ) return p+1;
	return NULL;
}

/* consume all printable characters */
char* allString(char *p) {
	if (p==NULL || *p=='\0') return NULL;
	if (!isStringChar(p)) return p;
	return allString(p+1);
}

/* consume all printable characters, and get the consumed part */
char* getAllString(char *p, char* out) {
	char *e = p, o;
	if (p==NULL || *p=='\0') return NULL;
	p = allString(p);
	if (p==NULL || *p=='\0') return NULL;
	o = *p;
	*p = '\0';
	strcpy(out, e);
	*p = o;
	return p;
}

/* this parser consumes the input string, as long as the head is a digits.
 * it returns the rest of the string, where the head is certain to be a non-digit char */
char* allDigits(char *p) {
	if (p==NULL || *p=='\0') return NULL;
	if (!isDigit(p)) return p;
	return allDigits(p+1);
}

/* a parser that is composed of the previous parsers to get a number out of the input string.
 * it succeeds iff the input line contains a floating point number. if successful, the result
 * is given in res, if fails, the error is given in error and return value is NULL.*/
char* getInteger(char*p, int* res, int* err) {
	char* e;
	char t;

	/* do we have anything to work on? */
	if (p==NULL || *p == '\0') return NULL;

	/* a nice trick by using or parser */
	e = allDigits(or(oneOf(p, "+-"), p));

	/* at the end of the previous 4 lines, we could have numbers in the following format -1, 1, +1 */

	/* if any of the above parsers had failed, return failure */
	if (e == NULL) 	return e;

	/* remove leading whitespace */
	e = strip(e, " ");

	/* if after removing all whitespaces we didnt hit either ',', end-of-line, or nothing was consumed at all,
	 * then the input that was given is wrong! fail this parser!*/
	if ((!oneOf(e, "\n\r,"))||e==p) {
		*err = NUMBER_ERR;
		return NULL;
	}
	/* if everything succeeded up till now, then e points to the rest of the input AFTER the valid float number,
	 * so we use a little trick to make it a float number:*/
	t = *e;                    	/* save the end of the consumed string */
	*e = '\0';					/* mark it as end of line */
	*res = (float)atoi(p);		/* use atof to transform the original input string (p) into the number */
	*e=t;						/* set the end of consumed string back to its original value */
	return e;					/* return the pointer to the rest of the unconsumed string */
}

/* a parser that is composed of the previous parsers to get a number out of the input string.
 * it succeeds iff the input line contains a floating point number. if successful, the result
 * is given in res, if fails, the error is given in error and return value is NULL.*/
char* getNumber(char*p, float* res, int* err) {
	char* e;
	char t;

	/* do we have anything to work on? */
	if (p==NULL || *p == '\0') return NULL;

	/* a nice trick by using or parser */
	e = or(charIs(p, '-'), p);
	e = allDigits(e);
	e = or(charIs(e, '.'), e);
	e = or(allDigits(e), e);

	/* at the end of the previous 4 lines, we could have numbers in the following format -1, 1, -1. ,1.543, -65.45 */

	/* if any of the above parsers had failed, return failure */
	if (e == NULL) 	return e;

	/* remove leading whitespace */
	e = strip(e, " ");

	/* if after removing all whitespaces we didnt hit either ',', end-of-line, or nothing was consumed at all,
	 * then the input that was given is wrong! fail this parser!*/
	if ((!charIs(e, ',') && !charIs(e, '\n'))||e==p) {
		*err = NUMBER_ERR;
		return NULL;
	}
	/* if everything succeeded up till now, then e points to the rest of the input AFTER the valid float number,
	 * so we use a little trick to make it a float number:*/
	t = *e;                    	/* save the end of the consumed string */
	*e = '\0';					/* mark it as end of line */
	*res = (float)atof(p);		/* use atof to transform the original input string (p) into the number */
	*e=t;						/* set the end of consumed string back to its original value */
	return e;					/* return the pointer to the rest of the unconsumed string */
}

/* matchWord succeeds iff the input string contains exactly the string m */
char* matchWord(char* p, char* m) {
	if (p==NULL || *m=='\0') return p;
	if (*p==*m) return matchWord(p+1, m+1);
	else return NULL;
}

/* This method makes sure the string m is matched within the beginning of string p, and that it ends with either
 * a whitespace or a carriage return, but on success it returns the string containing that last letter.
 */
char* matchWordD(char* p, char* m) {
	char* e;
	return oneOf((e=matchWord(p, m)), "\r\n ") ? e : NULL;
}

