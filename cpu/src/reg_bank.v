`timescale 1ns / 1ps

/* ARM register bank */
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
    output wire [31:0] read_A_data,
           wire [31:0] read_B_data,
           wire [31:0] read_pc_data,
           wire  [3:0] read_cpsr_data,
           wire [15:0] debug_out_R14
    );
    
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
            if (write_pc_en && !(write_select == PC_SELECT && write_en))
                BANK[PC_SELECT] <= write_pc_data;
            
            /* Normal register write */
            else if (write_en)
                BANK[write_select] <= write_data;
        end
    end
    
    /* read lines */
    assign read_A_data    = BANK[read_A_select];
    assign read_B_data    = read_B_en ? BANK[read_B_select] : 32'bz;
    assign read_pc_data   = BANK[PC_SELECT];
    assign read_cpsr_data = cpsr;
    
    assign debug_out_R14  = BANK[4'd14][15:0];

endmodule
