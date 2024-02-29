add $5, $0, $0 ; index of current element
add $8, $1, $0 ; storing array start address
lis $4
.word 4 ; constant for 4
lis $21 ; constant 1
.word 1

loop: 
;check loop exit condition, adding 1 because number of comparisons is one less than number of elements
add $5, $5, $21 ; current element 
beq $5, $2, end1 ; if current element is the last element end loop 
sub $5, $5, $21 ; restore index

; store values in $1, $2, $31
sw $1, -4($30) ; store array address 
sw $2, -8($30) ; store number of elements
sw $31, -12($30) ; store return address
lis $3
.word 12
sub $30, $30, $3 ; update stack pointer
;set $1 to current array element 
mult $4, $5 ; multiply index by 4
mflo $6
add $1, $6, $8 ; add index to array address
lw $1, 0($1) ; load value at current array element
add $5, $5, $21 ; increment index to get next element, no need to restore after since we need to increment anyways
mult $4, $5 ; multiply index by 4
mflo $6
add $2, $6, $8 ; add index to array address to get i+1th element
lw $2, 0($2) ; load value at i+1th element
; compare
lis $22
.word compare ; load address of compare 
jalr $22
;check if we want to exit
beq $3, $0, end0
;restore values in $1, $2, $31
lis $1 
.word 12
add $30, $30, $1 ; update stack pointer
lw $1, -4($30) ; restore array address
lw $2, -8($30) ; restore number of elements
lw $31, -12($30) ; restore return address
beq $0, $0, loop ; continue loop

end0: 
lis $1 
.word 12
add $30, $30, $1 ; update stack pointer
lw $1, -4($30) ; restore array address
lw $2, -8($30) ; restore number of elements
lw $31, -12($30) ; restore return address
add $3, $0, $0 ; return 0
jr $31

end1: 
add $3, $21, $0 ; set return value to 1
jr $31
