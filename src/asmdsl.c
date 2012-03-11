/*
 * asmDsl.c
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */


#include "asmdsl.h"

CMD(mov) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), func(VAL(1), VAL(2), label));


void func(Operand oper1, Operand oper2, Label label) {
	if (label[0]!='\0')	printf("label %s\n", label);
	debugPrint(oper1);
	debugPrint(oper2);
}
void debugPrint(Operand oper) {
	switch (oper.kind) {
		case reg:
			printf("got register %i\n", oper.get.reg);
			break;
		case constant:
			printf("got constant %i\n", oper.get.constant);
			break;
		case direct:
			printf("got direct %s\n", oper.get.direct);
			break;
		case label_with_index:
			printf("got label with index %s %s\n", oper.get.oneIndex.label, oper.get.oneIndex.index);
			break;
		case label_with_two_indices:
			printf("got label with two index %s %s %d\n", oper.get.twoIndice.label, oper.get.twoIndice.index, oper.get.twoIndice.reg);
			break;
		default:
			printf("error!");
			break;
	}
}

