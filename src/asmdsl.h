/*
 * asdDsl.h
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */

#ifndef ASMDSL_H_
#define ASMDSL_H_

#include "matdsl.h"
#include "parser.h"

/*

#define NONE(A) { A; }
PF(stop) NONE(generateStop)

#define REG(v,code) { \
	int v;\
	TO_PROCESS;\
	if(((p=isDigit(charIs(strip(p," "),'r')))){\
		code;\
	}\
	return p;\
}

*/
/*CMD(mov) TWO(OR(CONST(c), OR(label(l), REG(r1))), REG(r2), generateMov(c,l,r1,r2))

#define OR(X,Y,C) { \
}
*/

#endif /* ASMDSL_H_ */
