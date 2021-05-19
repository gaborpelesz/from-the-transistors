`timescale 1ns / 1ps

/* ARM register bank

    - HOW TO CONNECT?
        The register bank has two read buses (A and B) and one write bus.
        The read bus A is directly connected to the ALU, while B is connected
        to the shifter. (op1 and op2)
        The write bus (the output) is connected to the output of the ALU.
    
    - Note: Writing happens on posedge, reading should happen 
            on negedge outside while using this module

*/
module reg_bank(
    input  wire        clk,
           wire  [3:0] read_A_select,
           wire  [3:0] read_B_select,
           wire        read_B_en,     // output to the B bus should be tri-state
           wire  [3:0] write_select,
           wire        write_en,
           wire [31:0] write_data,
           wire        write_pc_en,
           wire [31:0] write_pc_data,
           wire  [3:0] write_cpsr_data,
           wire        write_cpsr_en,
           wire        reset,
    output reg  [31:0] read_A_data,
           reg  [31:0] read_B_data,
           reg  [31:0] read_pc_data,
           reg   [3:0] read_cpsr_data,
           reg  [15:0] debug_out_R14
    );
    
    localparam PC_SELECT = 4'd15;
    
    reg [31:0] BANK [0:15]; // 16 x 32bit registers R0-R15, where R15 is the PC
    reg  [3:0] cpsr = 4'b0000;        // reduced Current Program Status Register, leaving only the N,Z,C,V bits

 
    integer i = 0;
    always @ (posedge clk)
    begin
        /* Reset register bank */
        if (reset)
        begin
            for (i = 0; i < 16; i = i + 1)
            begin
                BANK[i] <= 32'b0;
            end
        end
        else
        begin
            // deal with status register
            if (write_cpsr_en)
                cpsr <= write_cpsr_data;

            /* Write to PC from Address incrementer */
            // only write PC if we currently don't modify it from the ALU
            if (write_pc_en && !(write_select == PC_SELECT && write_en))
                BANK[PC_SELECT] <= write_pc_data;
            
            /* Normal register write */
            else if (write_en)
                BANK[write_select] <= write_data;
        end
    end
    
    always @ (negedge clk)
    begin
        read_cpsr_data <= cpsr;
    
        // B bus tri-state
        if (read_B_en) 
            read_B_data <= BANK[read_B_select];
        else
            read_B_data <= 32'bz;

        read_A_data <= BANK[read_A_select];
        read_pc_data <= BANK[PC_SELECT];
            
        /* DEBUG OUTPUT */
        debug_out_R14 <= BANK[4'd14][15:0];
    end
    


endmodule
