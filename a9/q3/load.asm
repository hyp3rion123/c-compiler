.import readWord
.import printHex

; constants
lis $4
.word 4
lis $8
.word 8
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
add $24, $3, $0
add $25, $3, $0 ; store offset into footer so that we can access later when relocating
;add $25, $25, $4 ; doesn't work without this for some reason?

;get 3rd instruction, store rel count in $24
jalr $10
add $20, $3, $0
sub $24, $24, $20 ;size of footer
div $24, $4
mflo $24 ; # of instructions in footer

;store instr count in 20
sub $20, $20, $12
div $20, $4
mflo $20

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

;relocate
;test reading and printing the instr in footer, use $24 as itr
;add $22, $25, $0
footerLoop:
jalr $10
bne $3, $13, endloop

rel:
jalr $10
add $1, $3, $0
add $1, $21, $1 ; a + rel 
sub $1, $1, $12 ; a + rel - 12
lw $9, 0($1) ; $9 = MEM[a + rel - 12]

add $9, $21, $9 ; MEM[a + rel - 12] += a
sub $9, $9, $12 ; MEM[a + rel - 12] -= 12
sw $9, 0($1) ; MEM[a + rel - 12] += a - 12

endloop:
add $22, $22, $4
sub $24, $24, $13
bne $24, $0, footerLoop

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
sw $24, -4($30)
sub $30, $30, $4
sw $25, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4

; execute loaded code
jalr $21

; restore saved registers
lis $4
.word 4
lis $8
.word 8
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
lw $24, -4($30)
sub $30, $30, $4
lw $25, -4($30)
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
