beq $1, $0, 3
divu $2, $1
mfhi $10
beq $10, $0, skip
add $3, $0, $0
jr $31
skip:
lis $11
.word 1
add $3, $11, $0
jr $31
