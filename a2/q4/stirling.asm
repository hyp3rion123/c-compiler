stirling: 
;save registers 
sw $11, -4($30)
sw $1, -8($30)
sw $2, -12($30)
sw $4, -16($30)
sw $5, -20($30)
sw $31, -24($30)
lis $11
.word 24
sub $30, $30, $11

;constants 
lis $11
.word 1 ; constant 1

;BASE CASES
bne $1, $0, checkKelseN

; n=0
beq $2, $0, return1
beq $0, $0, return0

checkKelseN: ; n!=0
bne $2, $0, recurse

return0:
add $3, $0, $0
beq $0, $0, end

return1:
add $3, $11, $0
beq $0, $0, end
;;BASE CASES DONE

recurse:
lis $5
.word stirling
; $1 = n-1
sub $1, $1, $11
; $3 = f(n-1, k)
jalr $5
multu $3, $1
mflo $4
; $2 = k-1
sub $2, $2, $11
; $3 = f(n-1, k-1)
jalr $5
; compute f(n-1, k-1) * (n-1) + f(n-1, k)
add $3, $3, $4

end: 
;restore registers
lis $11
.word 24
add $30, $30, $11
lw $11, -4($30)
lw $1, -8($30)
lw $2, -12($30)
lw $4, -16($30)
lw $5, -20($30)
lw $31, -24($30)
jr $31

