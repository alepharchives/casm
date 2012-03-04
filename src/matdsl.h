/*
 * matdsl.h
 *
 *  Created on: Dec 28, 2011
 *      Author: shlomi vaknin (039014220)
 *
 *  Our DSL definition. the dsl is made up of two layers:
 *  directives - the dsl commands, such as PARSE, FMAT, CMD etc. these divide into two categories:
 *  	internal directives - ones that are used to deal with user input, (FMAT, MAT, SCALAR, etc)
 *  	external directives - these are PARSE, TRY and ELSE - are used to call the commands.
 *
 *  commands   - the commands the user can actually type,
 *  				these are always defined by the CMD directive, and are defined in terms of
 *  				internal directives. the name of each commands has to be the name the user
 *  				will use. (e.g.  print_mat, mul_mat, etc..)
 *
 *	the idea is to use the internal directives to build commands in a composable form,
 *	then, once the commands are properly built, use the external directives to expose them
 *	to the user.
 */

#ifndef MATDSL_H_
#define MATDSL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mat.h"
#include "parser.h"

/* define the maximum number of matrices used, and declare that someone will provide them */
#define MAX_MATS 6
extern Mat Mats[];

/* this is used to extend the parser error mechanism */
#define MAT_IDENT_ERR 10
#define VERIFY_ERR 11


/* external directives definition
 * ------------------------------
 * PARSE - the directive that marks the beginning of entry point of our dsl inside the interactive
 * 		   system. it puts the given line into the dsl context, and gets ready to TRY other directives.
 * TRY	 - a directive that attempts to fit the user input with the given cmd. failure to fit will
 * 		   continue to look at the next TRY.
 * ELSE  - if none of the TRY directives had matched, the givem msg will be printed to the screen
 *
 * a bit about the implementation: we define an error indicator, strip the line of spaces,
 * and start with an if(false), so each try could be formed as an 'else if' commands.
 * this way we can compose more and more TRY directives as we please.
 */
#define PARSE(line)  {int err=0;char*l=strip(line, " ");if(0) {}
#define TRY(cmd)      else if (cmd (matchWordD(l, #cmd) , &err)) {}
#define ELSE(msg)     else if (!err){ printf("%s\n",msg);}}

/* Internal directives
 * ------------------- */

/* VERIFY - makes sure that the user did not enter any extra parameters. if he did, a default error
 * 	 	 	will be displayed and an error code will be returned
 */
#define VERIFY { \
		if (*strip(p," ")!='\n') { \
			*err=VERIFY_ERR; \
			printf("too many parameters\n"); \
			return NULL; \
		} \
}

/* TO_PROCESS - this has to be defined at the beginning of every internal directive.
 * 				the mechanism actually steps into each function, but due to the composable
 * 				form, the input to the function (namely p) might indicate a previous error,
 * 				by either passing a finished string or a NULL pointer. if the previously composed
 * 				function had failed, this function will fail.
 *
 * 				in a more functional language i would have used an Option monad for this strategy.
 */
#define TO_PROCESS if (p==NULL || *p == '\0') return NULL;

/* CMD - this is a contract that defines a function that actually
 	 	handles an interactive command. in a better type-checked languages, DO and OR will only allow
 	 	PF function to be passed to them.
 */
#define CMD(name) char* name(char* p, int* err)


/* This section is more interesting, this is how we define the actual composable dsl directives:
 * each directive takes two parameters:
 *   v    - a name for the defined variable that holds the information this directive will retrieve
 *   code - an extra block of code, that will be executed. this what makes composition possible.
 *
 *   for examples for usages, see matdsl.c
 */


/*   FMAT - this directive will grab the next matrix identifier, NOT requiring it to have a ','
 * 			before it, and put the index of the matrix in v.
 *
 * 			this directive is special since it can must ONLY appear at the beginning of a new CMD
 * 			that needs to accept a matrix as its first parameter. if more parameters are needed,
 * 			then regular MAT should be used. the F in FMAT is for First.
 */
#define FMAT(v,code) { \
	int v;\
	TO_PROCESS;\
	if(((p=getMatrixIndex(strip(p," "), &v, #v, err)))){\
		code;\
	}\
	return p;\
}

/*
 *	MAT - this is very similar to FMAT, it grabs the next matrix identifier, REQUIREING it to
 *		  have a ',' somewhere in the line before the identifier, and puts its index into v.
 *
 *		  we can compose as many of these as we want, but cant use this as a first directive in a cmd.
 *		  to get a matrix in the first param, we must use FMAT.
 */
#define MAT(v, code) {\
	int v;\
	if(((p=getMatrixIndex(strip(charIsE(strip(p, " "), ',', err)," "), &v, #v, err)))) {\
		code; \
	}\
	else {\
		if (*err==CHARIS_ERR) printf("wrong parameters, %s parameter must be separated by ','\n", #v);\
		return NULL;\
	}\
}

/*
 *	SCALAR - this directive extracts a float number out of the user input, has to have a ',' before it.
 *	 	 	 this directive parses a number, storing it in v.
*/
#define SCALAR(v, code) {\
	float v;\
	if(((p = getNumber(strip(charIsE(strip(p, " "), ',', err)," "), &v, err)))) {\
		code; \
	}\
	else {\
		if (*err==CHARIS_ERR) printf("wrong parameters, %s parameter must be separated by ','\n", #v);\
		return NULL;\
	}\
}

/*
 *	OSCALAR - same as SCALAR, but the input is OPTIONAL. if not supplied, v is set to 0.
 */
#define OSCALAR(v, code) {\
	float v=0;\
	if (*strip(p," ")!='\n') { \
		p = getNumber(strip(charIsE(strip(p, " "), ',', err)," "), &v, err); \
		if (*err==CHARIS_ERR) {\
			printf("wrong parameters, %s parameter must be separated by ','\n", #v);\
			return NULL;\
		} else if (*err==NUMBER_ERR) {\
			printf("Wrong parameters, %s parameter must be a real number\n", #v);\
			return NULL;\
		}\
	}\
	code; \
}

/* a helper function that will actually extract the matrix index out of the user input */
char* getMatrixIndex(char* p, int* matIndex, char* errStr, int* err);

/* finally, we declare all the commands our DSL exposes */
CMD(read_mat);
CMD(add_mat);
CMD(sub_mat);
CMD(mul_mat);
CMD(trans_mat);
CMD(print_mat);
CMD(mul_scalar);
CMD(stop);

#endif /* MATDSL_H_ */
