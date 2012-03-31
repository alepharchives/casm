/*
 * commons.h
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi.v
 *
 *  This file contains the definitions the data structures, enums and constants used in this project.
 */

#ifndef COMMONS_H_
#define COMMONS_H_

#include "DoubleLinkedList.h"

#define MAXLINE 82
#define MAX_LABEL 30

typedef unsigned char byte;

/* this enum is used to define the kind of addressing used */
typedef enum {
	constant=0,
	direct=1,
	label_with_index=2,
	label_with_two_indices=3,
	reg=4,
	none=7
} addr;

/* we typedef each different thing, to help the type-checker help us */
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

/* this is an actual op-code structure. it is using bit-fields for cooler bit access */
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

/* this is the context for an assembly generation. everything that is going on is stored in this structure, and this is the structure that
 * gets passed around to most of the functions */
typedef struct {
	list codeList;
	list allLabels;
	list deferred;
	list externlabels;
	int lastAsmOffset, lastDataOffset;
} Context ;
#endif /* COMMONS_H_ */
