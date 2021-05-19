`timescale 1ns / 1ps

module imm_data_provider(
    input  wire        clk,
           wire [31:0] in_immediate,
           wire        in_output_en,
    output reg  [31:0] out_b_bus
    );
    
    always @ (negedge clk)
        out_b_bus <= in_output_en == 0 ? 32'bz : in_immediate;

endmodule
