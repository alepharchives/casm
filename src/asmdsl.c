/*
 * asmDsl.c
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */


#include "asmdsl.h"
char* trimNewline(char* line)
{	line[strlen(line)-1]='\0';
	return line;
}

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
	int i=0;
	if (label[0]!='\0')	printf("label %s\n", label);
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

ASSIGN2(mov, MOV)
ASSIGN2(cmp, CMP)
ASSIGN2(add, ADD)
ASSIGN2(sub, SUB)
ASSIGN2(lea, LEA)

ASSIGN1(prn, PRN)
ASSIGN1(not, NOT)
ASSIGN1(clr, CLR)
ASSIGN1(inc, INC)
ASSIGN1(dec, DEC)
ASSIGN1(jmp, JMP)
ASSIGN1(bne, BNE)
ASSIGN1(red, RED)
ASSIGN1(jsr, JSR)

ASSIGN0(rts, RTS)
ASSIGN0(stop, STOP)

/* code in the table on page 33 */
CMD(cmp) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR5(REG, DIRECT, LABEL1, LABEL2, CONST),  GEN2(cmp))
CMD(mov) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		GEN2(mov))
CMD(add) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		GEN2(add))
CMD(sub) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 		GEN2(sub))
CMD(lea) TWO(OR3(DIRECT, LABEL1, LABEL2),             OR4(REG, DIRECT, LABEL1, LABEL2), 		GEN2(lea))

CMD(prn) ONE(OR5(REG, DIRECT, CONST,  LABEL1, LABEL2),		GEN1(prn))
CMD(not) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				GEN1(not))
CMD(clr) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				GEN1(clr))
CMD(inc) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				GEN1(inc))
CMD(dec) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				GEN1(dec))
CMD(jmp) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				GEN1(jmp))
CMD(bne) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				GEN1(bne))
CMD(red) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),				GEN1(red))
CMD(jsr) ONE(DIRECT,										GEN1(jsr))

CMD(rts)  NONE(GEN0(rts))
CMD(stop) NONE(GEN0(stop))

CMD(entry) ONE(DIRECT, GEN1(entry))
CMD(_extern) ONE(DIRECT, GEN1(extern_))

CMD(data) MANY_NUMBERS(CALL_DATA(data))
CMD(string) STRING(CALL_STRING(string))
