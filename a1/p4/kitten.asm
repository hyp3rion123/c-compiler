add $3, $0, $0
lis $20
.word 1
lis $10
.word 0xffffffff
lis $11 
.word 0xffff0004
lis $12
.word 0xffff000c
lw $1, 0($11)
beq $1, $10, skip
main:
sw $1, 0($12)
add $3, $3, $20
lw $1, 0($11)
bne $1, $10, main
skip:
jr $31
