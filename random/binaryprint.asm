lis $1
.word -100
lis $10
.word 0xffff000c
add $2, $1, $0
sw $2, 0($10)
jr $31
