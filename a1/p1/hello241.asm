add $2, $1, $0
lis $10
.word 1
lis $11
.word -241
mult $10, $11
mflo $4
add $1, $2, $4
jr $31

