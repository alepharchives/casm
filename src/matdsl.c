/*
 * matdsl.c
 *
 *  Created on: Dec 28, 2011
 *      Author: shlomi vaknin
 *
 *  all dsl commands implementations
 */

#include "matdsl.h"

/* these three commands are very basic, notice how declarative their implementations are: */

/* print_mat - take a first matrix, store its index in 'first',
 * 				then, VERIFY that we have no more arguemtns,
 * 				and call printMat on this matrix.
 *
 */
CMD(print_mat)  FMAT(first, VERIFY;printMat(Mats[first]))

/* mul_scalar - a more interesting implementation:
 * 				 take a matrix, store its index in 'first',
 * 				 take a scalar value, store it in 'second',
 * 				 take a matrix, store its index in 'third',
 * 				 call mulScalar on these parameters
 */
CMD(mul_scalar) FMAT(first, SCALAR(second, MAT(third, VERIFY;mulScalar(Mats[first], second, &Mats[third]))))

/* stop is a good example on how we can also embed regular code in our commands */
CMD(stop)       {TO_PROCESS; exit(0);}

/* the next three commands (add_mat, sub_mat and mul_mat) are very similar, they all take exactly
 * 3 matrices, and call a function that have the same signature, so we show the real strength of
 * our compositional design here, by defining a larger internal directive that is directly
 * composed out of the smaller directives, and then used to define each of the tree commands
 */
#define THREE_MATRIX(f) FMAT(first, MAT(second, MAT(third, VERIFY; f(Mats[first], Mats[second], &Mats[third]))))

CMD(add_mat)    THREE_MATRIX(add)
CMD(sub_mat)    THREE_MATRIX(sub)
CMD(mul_mat)    THREE_MATRIX(mul)

/* similar to the above, but this time defined directly */
CMD(trans_mat)  FMAT(first, MAT(second, VERIFY; transMat(Mats[first], &Mats[second])))

/* This command show just how composable our design is! we use the Optional scalar directive
 * to build a function that essentially takes in 17 parameters, but all 16 of them are optional,
 * it then directly calls the initMat function will all the defined variables.
 */
CMD(read_mat)   FMAT(first,
				OSCALAR(second,   OSCALAR(third,    OSCALAR(forth,    OSCALAR(fith,
				OSCALAR(sixth,    OSCALAR(seventh,  OSCALAR(eighth,   OSCALAR(ninth,
				OSCALAR(tenth,    OSCALAR(eleventh, OSCALAR(twelvth,  OSCALAR(thirtenth,
				OSCALAR(fortenth, OSCALAR(fiftenth, OSCALAR(sixtenth, OSCALAR(sevententh,
				initMat(&Mats[first], second,   third,    forth,    fith,
									  sixth,    seventh,  eighth,   ninth,
									  tenth,    eleventh, twelvth,  thirtenth,
									  fortenth, fiftenth, sixtenth, sevententh);)))))))))))))))))

/* a helper function to actually retrieve the matrix index from the user input, and handle
 * error cases.
 */
char* getMatrixIndex(char* p, int* matIndex, char* errStr, int* err) {
	/* strip all spaces, then match on 'MAT_' */
	p = matchWord(strip(p, " "), "MAT_");

	/* if matching failed, print an error message and return an error code */
	if (p == NULL) {
		printf("Wrong parameters, %s parameter must be a valid matrix identifier\n", errStr);
		*err = MAT_IDENT_ERR;
		return NULL;
	}
	/* if matching succeed, take the next char, and subtract 'A', to get its index */
	*matIndex = *(p++)-'A';

	/* if we were asked for a matrix out of our boundary, show an error and return an error code */
	if (*matIndex<0||*matIndex>MAX_MATS-1) {
		printf("Wrong parameters, matrix identifier has to be between MAT_A and MAT_F\n");
		*err = MAT_IDENT_ERR;
		return NULL;
	}
	return p;
}
