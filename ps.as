.entry STRADD
.entry MAIN
.extern REVERSE
.extern PRTSTR
.extern COUNT
STRADD: .data 1
STR: .string "abcdef"
LASTCHAR: .data 0
LEN: .data 0
K: .data 0

MAIN: lea STR[%LEN], STRADD
jsr COUNT
jsr PRTSTR
mov STRADD, [K]LASTCHAR[r3]
mov STR[%K], r7
add COUNT[%K], r3
dec LASTCHAR[%K]
inc K
jsr REVERSE
jsr PRTSTR
stop
