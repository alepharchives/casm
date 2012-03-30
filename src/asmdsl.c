/*
 * asmDsl.c
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */

#include "asmdsl.h"

/* assign the functions that will actually generate the op codes for each command*/
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

/* code in the table on page 33
 * this defines the parsers for each command, where at the end of each parser, we supply the function which
 * generates the actual op-codes (defined above) */
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

CMD(data) /*MANY_NUMBERS(CALL_DATA(data))*/
{
	char* l;
	int* nums;
	int i,count=0;
	if (p==((void *)0)) return ((void *)0);
	l = p = strip(p," ");
	if (oneOf(l, "\n\r")) {
		*err = EMPTY_DATA;
		return NULL;;
	}
	do {
		count++;
		l = getInteger(strip(l," "), &i, err);
		if (*err==NUMBER_ERR) {
			*err = BAD_DATA;
			return NULL;
		}
	} while (charIs(l, ','));
	if (!oneOf(l, "\r\n")) {
		*err = BAD_DATA;
		return NULL;
	}
	nums = (int*)malloc(sizeof(int)*count);
	i=0;
	l=p;
	while ((l=oneOf(getInteger(strip(l," "), &nums[i++], err), ",\n"))!=((void *)0));
	data_gen(context, label, nums, count, lineNumber,originalLine);
	return p;
}
CMD(string) STRING(CALL_STRING(string))
