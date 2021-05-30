MOV R0, #56, 16 ; constructing UART TX address: 0x0038_0000
MOV R1, #72     ; creating ASCII letter 'H' (H == 0x48)
STR R1, [R0]    ; sending an UART byte through MMIO

; NOP the rest
nop: MOV R0, R0     ; else NOP => No OPeration
     B nop          ; NOP forever
