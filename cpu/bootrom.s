;--------------------------------------------------
; MAIN - bootrom - receiving code through UART
;--------------------------------------------------

main_loop_init:
    MOV R13, #56, 16     ; UART TX address:     0x0038_0000
                         ; UART RX address:     0x0038_0001
                         ; UART status address: 0x0038_0002
                         ; UART signal data has been read: 0x0038_0003
    ORR R12, R13, #4, 24 ; BRAM base address:   0x0038_0400
    MOV R11, R12         ; BRAM current address

    ; constructing MOV PC, #0 decoded instruction: 0xe3a0_f000
    ; this will be added explicitly as the last instruction in BRAM
    MOV R10, #227, 8       ; 0xe300_0000
    ORR R10, R10, #160, 16 ; 0xe3a0_0000
    ORR R10, R10, #240, 24 ; 0xe3a0_f000

main_loop:
    receive_instructions:
        MOV R0, R1
        MOV R1, #0                          ; contains the received instruction after "receive_bytes"
        MOV R2, #0                          ; received bytes counter

        receive_bytes:
            uart_rx_block_until_new_data: 
                LDR R4, [R13, #+2]               ; load UART status
                TST R4, #4                       ; check if rx_data_valid is set -> if result == 0 then not set
                BEQ uart_rx_block_until_new_data ; if data is not valid then branch back to check again

            LDR R4, [R13, #+1]              ; load received data
            STR R4, [R13, #+3]              ; signal UART that we have read the new data

            ORR R1, R4, R1, LSL #8          ; add incoming byte to container
            ADD R2, R2, #1                  ; increment received byte counter

            CMP R2, #4
            BNE receive_bytes               ; test if all 4 bytes of the instruction has arrived, if not then loop back

        CMP R1, #0                          ; test if this is the last instruction (UART should send 32'b0 if finished)
        BEQ run_program                     ; if we received the last instruction indicater, then run the program

        STR R1, [R11], #+4                  ; else saving the instruction in BRAM, increment current instruction pointer
        B receive_instructions              ; get back to receive more instructions

    run_program:
        STR R10, [R11], #+4                 ; add last instruction to program: MOV PC, #0
        MOV R13, R11                        ; save the address of the last instruction + 4
        MOV PC, R12                         ; start program execution by branching at the BRAM base address
