; begin prologue
.import print
lis $4
.word 4
lis $11
.word 1
sub $29, $30, $4 ; setup frame pointer
sw $1, -4($30)
sub $30, $30, $4 ; update stack pointer
sw $2, -4($30)
sub $30, $30, $4 ; update stack pointer

; end prologue

sw $1, -4($30)
sub $30, $30, $4 ; update stack pointer
lw $3, 0($29) ; <-- code(a)
sw $3, -4($30)
sub $30, $30, $4 ; update stack pointer
lw $3, -4($29) ; <-- code(b)
add $30, $30, $4 ; update stack pointer
lw $5, -4($30)
add $3, $5, $3
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4 ; update stack pointer
lis $5
.word print
jalr $5
add $30, $30, $4 ; update stack pointer
lw $31, -4($30)
add $30, $30, $4 ; update stack pointer
lw $1, -4($30)
lis $3
.word 0
; begin epilogue
jr $31
