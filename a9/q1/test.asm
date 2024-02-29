.import readWord
.import printHex

lis $10
.word readWord
lis $11
.word printHex

jalr $10 
add $1, $3, $0;;copy to $1 to print
jalr $11

jalr $10 
add $1, $3, $0;;copy to $1 to print
jalr $11

jalr $10 
add $1, $3, $0;;copy to $1 to print
jalr $11

jalr $10 
add $1, $3, $0;;copy to $1 to print
jalr $11

jalr $10 
add $1, $3, $0;;copy to $1 to print
jalr $11