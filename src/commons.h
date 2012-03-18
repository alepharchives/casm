/*
 * commons.h
 *
 *  Created on: 18 במרס 2012
 *      Author: ypsw
 */

#ifndef COMMONS_H_
#define COMMONS_H_

#include "DoubleLinkedList.h"

typedef unsigned char byte;
#define MAX_LABEL 30

typedef enum {
	constant=0,
	direct=1,
	label_with_index=2,
	label_with_two_indices=3,
	reg=4,
	none=7
} addr;

typedef int Reg;
typedef int Constant;
typedef char Label[MAX_LABEL];
typedef Label Direct;
typedef struct  {
		Label label;
		Label index;
	} LabelOneIndex;
typedef struct  {
		Label index;
		Label label;
		Reg reg;
	} LabelTwoIndice;

typedef struct {
	addr kind;
	union {
		Reg reg;
		Constant constant;
		Direct direct;
		LabelOneIndex oneIndex;
		LabelTwoIndice twoIndice;
	} get;
} Operand;

typedef
	union {
	int code;
	struct {
		int destReg:3;
		int destKind:3;
		int sourceReg:3;
		int sourceKind:3;
		int op:4;
	}bit;
} OpCode ;

typedef struct {
	list codeList;
	list allLabels;
	list deferred;
	list dataList;
} Context ;
#endif /* COMMONS_H_ */
