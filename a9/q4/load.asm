.import readWord
.import printHex

; constants
lis $4
.word 4
lis $10
.word readWord
lis $11
.word printHex
lis $12
.word 0x0C
lis $13
.word 1
lis $30
.word 0x00fffffc

;save $31
sw $31, -4($30)
sub $30, $30, $4

;;get mem address = a, save in $21
jalr $10
add $21, $3, $0

;flush first instruction
jalr $10
;get 2nd instruction, compute instr count
jalr $10
add $20, $3, $0
sub $20, $20, $12
div $20, $4
mflo $20

;flush 3rd instruction
jalr $10

;;print first time while storing in mem address at 

add $22, $21, $0
add $23, $0, $0 ; itr to be used for loop2

loop:
jalr $10
sw $3, 0($22) ;;save to address w.r.t a
add $22, $22, $4 ;;increment address reference
add $1, $3, $0;;copy to $1 to print
jalr $11
sub $20, $20, $13
add $23, $23, $13
bne $20, $0, loop

; save registers being used before executing code that might change them, store at a + i
;load $31
add $30, $30, $4
lw $31, -4($30)

sw $20, -4($30)
sub $30, $30, $4
sw $21, -4($30)
sub $30, $30, $4
sw $22, -4($30)
sub $30, $30, $4
sw $23, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4

; execute loaded code
jalr $21

; restore saved registers
lis $4
.word 4
lis $10
.word readWord
lis $11
.word printHex
lis $12
.word 0x0C
lis $13
.word 1
lis $14
.word 0x10000
lis $30
.word 0x00fffffc
lw $20, -4($30)
sub $30, $30, $4
lw $21, -4($30)
sub $30, $30, $4
lw $22, -4($30)
sub $30, $30, $4
lw $23, -4($30)
sub $30, $30, $4
lw $31, -4($30)
sub $30, $30, $4



;store $31
sw $31, -4($30)
sub $30, $30, $4


;loop through a and print again
add $22, $21, $0
loop2:
lw $1, 0($22)
jalr $11
add $22, $22, $4
sub $23, $23, $13
bne $23, $0, loop2

;load $31
add $30, $30, $4
lw $31, -4($30)

jr $31
