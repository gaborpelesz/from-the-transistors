     MOV R0, #0     ; R0 = 0
inc: ADD R0, R0, #1 ; ++R0
     TEQ R0, #39    ; R0 == 39
     BNE inc        ; if (R0 != 39) goto inc;
nop: MOV R0, R0    ; else NOP => No OPeration
     B nop          ; NOP forever