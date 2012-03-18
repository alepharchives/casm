/*
 * asmDsl.c
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */


#include "asmdsl.h"

void cmp_func(Operand oper1, Operand oper2, Label label) {
	printf("CMP\n");
	if (label[0]!='\0')	printf("label %s\n", label);
	debugPrint(oper1);
	debugPrint(oper2);
}

void mov_func(Operand oper1, Operand oper2, Label label) {
	printf("MOV\n");
	if (label[0]!='\0')	printf("label %s\n", label);
	debugPrint(oper1);
	debugPrint(oper2);
}

void add_func(Operand oper1, Operand oper2, Label label) {
	printf("ADD\n");
	if (label[0]!='\0')	printf("label %s\n", label);
	debugPrint(oper1);
	debugPrint(oper2);
}

void debugNums(char* label, int* nums, int size) {
	if (label[0]!='\0')	printf("label %s\n", label);
	int i=0;
	while (i<size) {
		printf("%d, ", nums[i++]);
	}
}

void debugString(char* label, char* text) {
	if (label[0]!='\0')	printf("label %s\n", label);
	printf("%s\n", text);
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

/* code in the table on page 33 */
CMD(cmp) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR5(REG, DIRECT, LABEL1, LABEL2, CONST),  cmp_func(VAL(1), VAL(2), label))
CMD(mov) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		mov_func(VAL(1), VAL(2), label))
CMD(add) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		add_func(VAL(1), VAL(2), label))
CMD(sub) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		add_func(VAL(1), VAL(2), label))
CMD(lea) TWO(OR3(DIRECT, LABEL1, LABEL2),             OR4(REG, DIRECT, LABEL1, LABEL2), 		add_func(VAL(1), VAL(2), label))

CMD(prm) ONE(OR5(REG, DIRECT, CONST,  LABEL1, LABEL2),		debugPrint(VAL(1)))
CMD(not) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(clr) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(inc) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(dec) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(jmp) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(bne) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(red) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				debugPrint(VAL(1)))
CMD(jsr) ONE(DIRECT,										debugPrint(VAL(1)))

CMD(rts)  NONE(printf("rts!"))
CMD(stop) NONE(printf("stop!"))

CMD(entry) ONE(DIRECT, genEntry(label, VAL(1), context))
CMD(_extern) ONE(DIRECT, genExtern(label, VAL(1), context))

CMD(data) MANY_NUMBERS(genData(label, nums, count, context))
CMD(string) STRING(debugString(label, text))
