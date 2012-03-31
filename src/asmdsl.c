/*
 * asmDsl.c
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi.v
 *
 *  This file is the most interesting one!
 *  if you run only the c preprocessor on this file, you will see that all the actual code of our app goes in here.
 *
 *  first, we will assign between a generating function and its defined command
 *  and second, we will hook these assigned generators into the parsers, so once a parser succeeds, it will simply call one of these generating functions
 */

#include "asmdsl.h"

/* assign the functions that will actually generate the codes for each command*/
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

/* lets take cmp as an example to explain each line:
 *   cmp is a command that takes in two operands, the first is one of five (reg, const, ...), and the second is also one of five.
 *   if this is the command you got, call generating function cmp (as defined in line ASSIGN2(cmp, CMP), which generates a cmp command with two operands.*/
PARSER(cmp) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR5(REG, DIRECT, LABEL1, LABEL2, CONST),  	GEN2(cmp))
PARSER(mov) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 			GEN2(mov))
PARSER(add) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 			GEN2(add))
PARSER(sub) TWO(OR5(REG, CONST, DIRECT, LABEL1, LABEL2), OR4(REG, DIRECT, LABEL1, LABEL2), 			GEN2(sub))
PARSER(lea) TWO(OR3(DIRECT, LABEL1, LABEL2),             OR4(REG, DIRECT, LABEL1, LABEL2), 			GEN2(lea))

PARSER(prn) ONE(OR5(REG, DIRECT, CONST,  LABEL1, LABEL2),											GEN1(prn))
PARSER(not) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),													GEN1(not))
PARSER(clr) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),													GEN1(clr))
PARSER(inc) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),													GEN1(inc))
PARSER(dec) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),													GEN1(dec))
PARSER(jmp) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),													GEN1(jmp))
PARSER(bne) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),													GEN1(bne))
PARSER(red) ONE(OR4(REG, DIRECT, LABEL1, LABEL2),													GEN1(red))
PARSER(jsr) ONE(DIRECT,																				GEN1(jsr))

PARSER(rts)  NONE(GEN0(rts))
PARSER(stop) NONE(GEN0(stop))

/* these two are slightly different, notice that we didnt define entry or extern_ in the assign2 statements above,
 * that is because we arent REALLY generating code here, so the abstractions made above did not fit here. */
PARSER(entry) ONE(DIRECT, GEN1(entry))
PARSER(_extern) ONE(DIRECT, GEN1(extern_))

/* these two are evern more different, since they cant be handled with ONE or GEN directives, so they are a special case.. */
PARSER(data) MANY_NUMBERS(CALL_DATA(data))
PARSER(string) STRING(CALL_STRING(string))
