     MOV R0, #20           ; R0 = 20
     
     ; constructing address in R13: 0x0038_0400
     MOV R13, #56, 16
     ORR R13, R13, #4, 8

     STR R0, [R13, #+4] ; address: 0x0038_0404

nop: MOV R0, R0     ; else NOP => No OPeration
     B nop          ; NOP forever