`timescale 1ns / 1ps

/* ARM register bank */
module reg_bank(
    input  wire        clk,
           wire  [3:0] read_A_select,
           wire  [3:0] read_B_select,
           wire  [3:0] read_C_select,
           wire        read_B_en,     // output to the B bus should be tri-state
           wire  [3:0] write_select,
           wire        write_en,
           wire [31:0] write_data,
           wire        write_pc_en,
           wire [31:0] write_pc_data,
           wire  [3:0] write_cpsr_data,
           wire        write_cpsr_en,
           wire        reset,
    output wire [31:0] read_A_data,
           wire [31:0] read_B_data,
           wire [31:0] read_C_data,
           wire [31:0] read_pc_data,
           wire  [3:0] read_cpsr_data,
           wire [15:0] debug_out
    );
    
    localparam R0  = 4'd00, R1  = 4'd01, R2  = 4'd02, R3  = 4'd03,
               R4  = 4'd04, R5  = 4'd05, R6  = 4'd06, R7  = 4'd07,
               R8  = 4'd08, R9  = 4'd09, R10 = 4'd10, R11 = 4'd11,
               R12 = 4'd12, R13 = 4'd13, R14 = 4'd14, R15 = 4'd15,
               LR  = 4'd14, PC  = 4'd15;
    localparam PC_SELECT = 4'd15;
    
    reg [31:0] BANK [0:15];    // 16 x 32bit registers R0-R15, where R15 is the PC
    reg  [3:0] cpsr = 4'b0000; // reduced Current Program Status Register, leaving only the N,Z,C,V bits

 
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
            if (write_pc_en && !(write_select == PC && write_en))
                BANK[PC] <= write_pc_data;
            
            /* Normal register write */
            if (write_en)
                BANK[write_select] <= write_data;
        end
    end
    
    /* read lines */
    assign read_A_data    = BANK[read_A_select];
    assign read_B_data    = read_B_en ? BANK[read_B_select] : 32'bz;
    assign read_C_data    = BANK[read_C_select];
    assign read_pc_data   = BANK[PC];
    assign read_cpsr_data = cpsr;
    
    assign debug_out  = BANK[R0][15:0];

endmodule
