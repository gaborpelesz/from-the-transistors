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
    input  wire clk,
           wire [3:0]  read_A_select,
           wire [3:0]  read_B_select,
           wire        read_B_en,     // output to the B bus should be tri-state
           wire [3:0]  write_select,
           wire        write_en,
           wire [31:0] write_data,
           wire        write_pc_en,
           wire [31:0] write_pc_data,
    output wire [31:0] read_A_data,
           reg  [31:0] read_B_data,
           wire [31:0] read_pc_data
    );
    
    localparam PC_SELECT = 4'd15;
    
    reg [31:0] BANK [0:15];
    
    /* Write */
    always @ (posedge clk)
    begin
        if (write_en)
            BANK[write_select] <= write_data;

        // only write PC if we currently don't modify it from the ALU
        if (write_pc_en && !(write_select == PC_SELECT && write_en))
            BANK[4'd15] <= write_pc_data;
            
        if (read_B_en) // B bus tri-state
            read_B_data <= BANK[read_B_select];
        else
            read_B_data <= 32'bz;
    end
    
    /* Read lines */
    assign read_A_data  = BANK[read_A_select];
    assign read_pc_data = BANK[PC_SELECT];

endmodule
