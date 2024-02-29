;$1 used to store current byte
add $3, $0, $0 ; byte count
lis $11 
.word 0xffff0004 ;Input address
lis $12
.word 0xffff000c ;output address
lis $13
.word 0xffffffff ;null char

;Read first char
lw $1, 0($11)

loop:
beq $1, $13, copy
sw $1, -4($30) ;store current word
lis $1
.word 4
sub $30, $30, $1 ; update stack pointer
lis $1
.word 1
add $3, $3, $1 ; increment word count
lw $1, 0($11) ; read next char
beq $0, $0, loop

copy:
;Copy value of $3 to $2
add $2, $3, $0

print:
beq $2, $0, end
lis $1
.word 4
add $30, $30, $1 ; update stack pointer
lw $1, -4($30) ; read next word
sw $1, 0($12) ; print word 
lis $1
.word 1
sub $2, $2, $1 ; decrement word count
beq $0, $0, print

end:
jr $31
