MOV R10, #56, 16 ; constructing UART TX address: 0x0038_0000

BL uart_tx_block_until_inactive

; SEND ('H')
MOV R0, #72     ; creating ASCII letter 'H' (H == 0x48)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('e')
MOV R0, #101    ; creating ASCII letter 'e' (e == 0x65)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('l')
MOV R0, #108    ; creating ASCII letter 'l' (l == 0x6c)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('l')
MOV R0, #108    ; creating ASCII letter 'l' (l == 0x6c)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('o')
MOV R0, #111    ; creating ASCII letter 'o' (o == 0x6f)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND (' ')
MOV R0, #32    ; creating ASCII letter ' ' (space == 0x20)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('w')
MOV R0, #119    ; creating ASCII letter 'w' (w == 0x77)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('o')
MOV R0, #111    ; creating ASCII letter 'o' (o == 0x6f)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('r')
MOV R0, #114    ; creating ASCII letter 'r' (r == 0x72)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('l')
MOV R0, #108    ; creating ASCII letter 'l' (l == 0x6c)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('d')
MOV R0, #100    ; creating ASCII letter 'd' (d == 0x64)
STR R0, [R10]   ; sending an UART byte through MMIO

BL uart_tx_block_until_inactive

; SEND ('!')
MOV R0, #33    ; creating ASCII letter '!' (! == 0x21)
STR R0, [R10]   ; sending an UART byte through MMIO

B finish_execution

;--------------------------------------------------
;--------------------------------------------------
; UART TX wait until gets inactive
;--------------------------------------------------

uart_tx_block_until_inactive: 
    LDR R1, [R10, #+2]
    TST R1, #2                       ; check if tx_is_active is set -> if R1 == 0 then not set
    BNE uart_tx_block_until_inactive ; while is_active branch back to check again
    MOV R15, R14                     ; return from routine (MOV PC, LR)

;--------------------------------------------------
;--------------------------------------------------
;--------------------------------------------------

finish_execution:
    MOV R0, R0

    MOV PC, #0