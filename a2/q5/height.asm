;constants
; $1 contains starting address of array
; $2 contains size of the array
lis $3 
.word 0 ; height of tree
lis $4
.word 0 ; current index
lis $10
.word -1 ; constant -1
lis $11
.word 1 ; constant 1
lis $14
.word 4 ; constant 4

;recursive procedure for determining height of current node
height:
; SAVE REGISTERS
sw $31, -4($30)
sw $20, -8($30)
sw $21, -12($30)
sw $15, -16($30)
sw $16, -20($30)
lis $28
.word 20
sub $30, $30, $28

; BASE CASES
; checking if both child nodes are -1
mult $14, $4
mflo $5 ; $5 contains increment for element we want
add $5, $5, $1 ; $5 now contains address of ith element
lw $20, 4($5) ; load left child
lw $21, 8($5) ; load right child
;;TEMP REMOVE WHEN DONE
add $18, $20, $0
add $19, $21, $0

beq $20, $10, checkRightOne
; left child isn't -1, $20 contains index of left child in array
beq $21, $10, recurseLeftOnly
beq $0, $0, recurse

checkRightOne:
beq $21, $10, return1
beq $0, $0, recurseRightOnly

return1:
sub $3, $0, $10 ; return 1
beq $0, $0, endProc

recurseLeftOnly:
;;TEMP REMOVE WHEN DONE
lis $26
.word 1
; change current index to left child
add $4, $0, $20 ; $4 contains index of left child
lis $25
.word height
jalr $25
; $3 contains height of left child
sub $3, $3, $10 ; $3 contains height of left child + 1 from root
beq $0, $0, endProc

recurseRightOnly:
;;TEMP REMOVE WHEN DONE
lis $26
.word 2
; change current index to right child
add $4, $0, $21 ; $4 contains index of right child
lis $25
.word height
jalr $25
; $3 contains height of right child
sub $3, $3, $10 ; $3 contains height of right child + 1 from root
beq $0, $0, endProc

recurse:
;;TEMP REMOVE WHEN DONE
lis $26
.word 3
add $4, $0, $20 ; $4 contains index of left child
lis $25
.word height
jalr $25
; $3 contains height of left child
add $15, $3, $0 ; move height of left child to $15
add $4, $0, $21 ; $4 contains index of right child
lis $25
.word height
jalr $25
; $3 contains height of right child
add $16, $3, $0 ; move height of right child to $16
slt $17, $15, $16 ; $17 contains 1 if left child is less than right child, 0 otherwise
beq $17, $11, returnrightside
;return left side
sub $3, $15, $10 ; $3 contains height of left child + 1 from root
beq $0, $0, endProc
returnrightside:
sub $3, $16, $10 ; $3 contains height of right child + 1 from root
beq $0, $0, endProc

endProc:
;RESTORE REGISTERS
lis $28
.word 20
add $30, $30, $28
lw $31, -4($30)
lw $20, -8($30)
lw $21, -12($30)
lw $15, -16($30)
lw $16, -20($30)
jr $31

; END OF HEIGHT PROCEDURE
