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


sw $29, -4($30)
sub $30, $30, $4 ; update stack pointer
sw $31, -4($30)
sub $30, $30, $4 ; update stack pointer
lw $3, 0($29) ; <-- code(a)
sw $3, -4($30)
sub $30, $30, $4 ; update stack pointer
lis $5
.word f
jalr $5
add $30, $30, $4 ; update stack pointer
lw $21, -4($30)
add $30, $30, $4 ; update stack pointer
lw $31, -4($30)
add $30, $30, $4 ; update stack pointer
lw $29, -4($30)
; begin epilogue
add $30, $30, $4
add $30, $30, $4
jr $31
f:
sub $29, $30, $4
sw $5, -4($30)
sub $30, $30, $4 ; update stack pointer
sw $6, -4($30)
sub $30, $30, $4 ; update stack pointer
sw $7, -4($30)
sub $30, $30, $4 ; update stack pointer
lw $3, 0($29) ; <-- code(c)
add $30, $30, $4 ; update stack pointer
lw $7, -4($30)
add $30, $30, $4 ; update stack pointer
lw $6, -4($30)
add $30, $30, $4 ; update stack pointer
lw $5, -4($30)
jr $31
