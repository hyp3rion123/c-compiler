; Prep special adds
lis $1
.word 1
lis $9
.word 0xffffffff ; reading null
lis $10
.word 0x0A ; line feed
lis $11
.word 0xffff0004 ; input
lis $12
.word 0xffff000c ; output
lis $13
.word 0x2D ; negative sign
lis $21
.word 0 ; first char of number
lis $22
.word 0 ; second char of number
lis $23
.word 0 ; third char of number
lis $24
.word 0 ; fourth char of number(if negative)
lis $25
.word -1 ; largest factor of 10
lis $28
.word 10 ; constant 10
lis $17 ; value to subtract to convert from ASCII to decimal 
.word 48

;store number
lw $21, 0($11)
;sw $21, 0($12)
lw $22, 0($11)
;sw $22, 0($12)
lw $23, 0($11)
;sw $23, 0($12)
lw $24, 0($11)
;sw $24, 0($12)

; count digits
beq $21, $10, donecount ; is line feed 
beq $21, $9, donecount ; is eof
beq $21, $13, 1 ; negative sign 
add $25, $25, $1 ; skips if first char is negative sign
beq $22, $10, donecount ; is line feed 
beq $22, $9, donecount ; is eof
add $25, $25, $1
beq $23, $10, donecount ; is line feed 
beq $23, $9, donecount ; is eof
add $25, $25, $1
beq $24, $10, donecount ; is line feed 
beq $24, $9, donecount ; is eof
add $25, $25, $1
;after this section, $25 stores the power of 10 of the number, min: 0, max: 2

donecount:
; calculate actual starting multiplier e.g 10, 100, 1
lis $26 ; multiplying factor 
.word 1
lis $27 ; constant 10
.word 10
lis $1 ; temp modifier
.word -1
beq $25, $0, donemultfactor ; stop when power of 10 is 0
multfactorloop:
mult $26, $27 ; multiply current factor by 10
mflo $26 ; replace current factor with higher power factor
add $25, $25, $1 ;decrement power of 10
bne $25, $0, multfactorloop ; iterate
;$26 contains the starting power of 10 we will multiply by e.g 1, 10, 100

donemultfactor: ; $26 contains 1, 10, or 100
; using $5 as final sum
lis $5
.word 0

; convert from ASCII to "decimal" value
sub $9, $9, $17 ; null/eof 
sub $10, $10, $17 ; line feed 
sub $13, $13, $17 ; negative sign
sub $21, $21, $17 
sub $22, $22, $17
sub $23, $23, $17
sub $24, $24, $17

;possible range: -128 : 255
beq $21, $13, proceedsign ; jumps to signed if negative
lis $1
.word 100
; check if multiplying factor is 100
bne $26, $1, proceedsign ; anything below 100 is signed

;possible range: 100:255
;check if $21 > 1
lis $1
.word 1
slt $2, $1, $21 ; $2 = 1 if 1 < first character
beq $2, $1, proceedunsign ; is unsigned if first character is a 2 * 100

;possible range: 100:199
lis $3
.word 2
slt $2, $3, $22 ; $2 = 1 if 2 < second character
beq $2, $1, proceedunsign

;possible range: 100:129
slt $2, $22, $3 ; $2 = 1 if second character < 2
beq $2, $1, proceedsign

;possible range: 120:129
lis $7
.word 7
slt $2, $7, $23 ; $2 = 1 if third character > 7
bne $2, $1, proceedsign ; 

proceedunsign:
lis $1
.word -1
;first digit
multu $26, $21
mflo $29 ; $29 storing product
add $5, $5, $29 ; $5 total sum storing new product
div $26, $28 ; reduce factor of 10
mflo $26 ; $26 contains lowered power of 10

;second digit
multu $26, $22
mflo $29 ; $29 storing product
add $5, $5, $29 ; $5 total sum storing new product

;third digit
add $5, $5, $23 ; $5 total sum storing new product
beq $0, $0, finishedsum ; finish processing the sum

proceedsign:
lis $1
.word -1
beq $21, $13, proceednegsign

;POSITIVE SIGNED INT
;first digit
mult $26, $21
mflo $29 ; $29 storing product
add $5, $5, $29 ; $5 total sum storing new product
div $26, $28 ; reduce factor of 10
mflo $26 ; $26 contains lowered power of 10

;second digit
beq $22, $10, finishedsum ; is line feed 
beq $22, $9, finishedsum ; is eof
mult $26, $22
mflo $29 ; $29 storing product
add $5, $5, $29 ; $5 total sum storing new product

;third digit
beq $23, $10, finishedsum ; is line feed 
beq $23, $9, finishedsum ; is eof
add $5, $5, $23 ; $5 total sum storing new product

beq $0, $0, finishedsum ; finish processing the sum

proceednegsign:
lis $1
.word -1
;first digit
mult $26, $22
mflo $29 ; $29 storing product
add $5, $5, $29 ; $5 total sum storing new product
div $26, $28 ; reduce factor of 10
mflo $26 ; $26 contains lowered power of 10
;sw $5, 0($12)

;second digit
beq $23, $10, finishedsumneg ; is line feed 
beq $23, $9, finishedsumneg ; is eof
mult $26, $23
mflo $29 ; $29 storing product
add $5, $5, $29 ; $5 total sum storing new product
;sw $5, 0($12)

;third digit
beq $24, $10, finishedsumneg ; is line feed 
beq $24, $9, finishedsumneg ; is eof
add $5, $5, $24 ; $5 total sum storing new product
;sw $5, 0($12)
beq $0, $0, finishedsumneg ; finish processing the sum

finishedsum:
;sw $21, 0($12)
;sw $22, 0($12)
;sw $23, 0($12)
;sw $24, 0($12)
sw $5, 0($12)
jr $31

finishedsumneg:
;convert to negative
;sw $5, 0($12)
lis $1
.word -1
mult $1, $5
mflo $5
;sw $21, 0($12)
;sw $22, 0($12)
;sw $23, 0($12)
;sw $24, 0($12)
sw $5, 0($12)
jr $31
