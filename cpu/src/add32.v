`timescale 1ns / 1ps

module add32(
    input  wire [31:0] in_data0, 
           wire [31:0] in_data1,
           wire        in_carry, // carry input, set it to HIGH to have no affect (in SUB)
    output wire [31:0] out_data,
           wire        out_carry, // carry output
           wire        out_overflow
    );
    
    wire [32:0] result;
    
    assign result       = in_data0 + in_data1 + in_carry;
    assign out_data     = result[31:0];
    
    assign out_carry    = result[32];
    
    assign out_overflow = ( in_data0[31] &  in_data1[31] & ~out_data[31]) | // both input is positive and result is negative
                          (~in_data0[31] & ~in_data1[31] &  out_data[31]);  // both input is negative and result is positive
    
endmodule
