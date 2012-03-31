/*
 * commons.h
 *
 *  Created on: 18 ���� 2012
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
typedef char OffsetBits[9];
typedef char AsmBits[25];
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

typedef enum {
	a = 'a',
	r = 'r',
	e = 'e'
} addrType;

typedef struct {
		int val;
		addrType type;
} addrVal;

typedef
	union {
	unsigned int code;
	struct {
		unsigned int destReg:3;
		unsigned int destKind:3;
		unsigned int sourceReg:3;
		unsigned int sourceKind:3;
		unsigned int op:4;
	}bit;
} OpCode ;

typedef struct {
	list codeList;
	list allLabels;
	list deferred;
	list externlabels;
	int lastAsmOffset, lastDataOffset;
} Context ;
#endif /* COMMONS_H_ */
