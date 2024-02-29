branchInstructions:
beq   $26, $27,  1
bne   $28, $29, branchInstructions
memoryInstructions:
bne   $28, $29, lastButNotLeast
sw    $31, -4($30)
lastButNotLeast: