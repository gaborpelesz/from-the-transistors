mov R1, #63, 28 ; rotate 63 by 28 bits on a 32bit word
mov R1, R2
mov R1, R2, LSL #2
add R1, R1, #32
add R1, R1, #63, 28 
add R1, R1, R2
add R1, R2, R3, LSL #4
cmp r1, r2
;add R2, R1, R2, LSR #5