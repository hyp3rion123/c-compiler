print: 
;store registers
sw $10, -4($30)
sw $11, -8($30)
sw $12, -12($30)
sw $13, -16($30)
sw $14, -20($30)
sw $15, -24($30)
sw $16, -28($30)
sw $2, -32($30)
sw $29, -36($30)
sw $1, -40($30)
lis $10
.word 40
sub $30, $30, $10

; constants
lis $10
.word 0xffff000c ; output address
lis $11
.word 0x0A ; line feed
lis $12
.word 48 ; '0'
lis $13 
.word 0x2D ; '-'
lis $14
.word -1
lis $15
.word 10
add $16, $0, $0 ; digit count
add $29, $0, $0 ; ram address to store digits

;if number is 0, print 0 and end
bne $1, $0, neg 
add $1, $1, $12 ; convert to ASCII
sw $1, 0($10) ; print first digit
beq $0, $0, end

neg:
;negative number logic
;check if $1 < 0
slt $2, $1, $0
beq $2, $0, store

sw $13, 0($10) ; print '-'
mult $1, $14
mflo $1

store:
div $1, $15
mfhi $1
sw $1, 0($29) ; store digit from last to first in ram 
lis $1
.word 4
add $29, $29, $1 ; update stack pointer
sub $16, $16, $14 ; increment digit count
mflo $1 ; get first n-1 digits back from quotient
bne $1, $0, store

output: ; collect digits from stack and print in lifo order
lis $1
.word 4
sub $29, $29, $1 ; update stack pointer
lw $1, 0($29) ; load digit from stack
add $1, $1, $12 ; convert to ASCII
sw $1, 0($10) ; print digit
add $16, $16, $14 ; decrement digit count
bne $16, $0, output

end:
sw $11, 0($10) ; line feed
;restore registers
lis $10
.word 40
add $30, $30, $10
lw $10, -4($30)
lw $11, -8($30)
lw $12, -12($30)
lw $13, -16($30)
lw $14, -20($30)
lw $15, -24($30)
lw $16, -28($30)
lw $2, -32($30)
lw $29, -36($30)
lw $1, -40($30)
jr $31



