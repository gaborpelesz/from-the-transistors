`timescale 1ns / 1ps

module imm_data_provider(
    input  wire [31:0] in_immediate,
           wire        in_output_en,
    output wire [31:0] out_b_bus
    );
    
    assign out_b_bus = in_output_en ? in_immediate : 32'bz;
    
endmodule
