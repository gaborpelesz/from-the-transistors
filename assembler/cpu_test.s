label: MOV R0, #5
MOV R1, #15
ADD R0, R0, R1  ;R0==20
MOV R5, R1      ;R5==15
ADD R0, R0, #1  ;++R0==21
SUB R14, R0, R5 ;R14==6
B label