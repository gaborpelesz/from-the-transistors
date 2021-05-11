`timescale 1ns / 1ps

/* ARM register bank

    - HOW TO CONNECT?
        The register bank has two read buses (A and B) and one write bus.
        The read bus A is directly connected to the ALU, while B is connected
        to the shifter. (op1 and op2)
        The write bus (the output) is connected to the output of the ALU.
    
    - Note: Writing happens on posedge, reading should happen 
            on negedge outside while using this module
            
    RELATED QUESTIONS:
        - The reg bank has 2 read inputs and outputs. What happens when we read 
          two registers in a decode phase, and when we want to read the PC to 
          prefetch the next instruction?
*/
module reg_bank(
    input  wire CLK,
    input  wire [3:0]  READ_A_SELECT, READ_B_SELECT, 
                       WRITE_SELECT, 
           wire        WRITE_EN,
           wire [31:0] WRITE_DATA,
           wire        WRITE_PC_EN,
           wire [31:0] WRITE_PC_DATA,
    output wire [31:0] READ_A_DATA, READ_B_DATA, READ_PC_DATA
    );
    
    localparam PC_SELECT = 4'd15;
    
    reg [31:0] BANK [0:15];
    
    /* Write */
    always @ (posedge CLK)
    begin
        if (WRITE_EN)
            BANK[WRITE_SELECT] <= WRITE_DATA;
        if (WRITE_PC_EN && WRITE_SELECT != PC_SELECT)
            BANK[4'd15] <= WRITE_PC_DATA;
    end
    
    /* Read lines */
    assign READ_A_DATA  = BANK[READ_A_SELECT];
    assign READ_B_DATA  = BANK[READ_B_SELECT];
    assign READ_PC_DATA = BANK[PC_SELECT];
    
    // assign pc = BANK[14];
endmodule
