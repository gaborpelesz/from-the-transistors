`timescale 1ns / 1ps

module mADD32(
    input  wire [31:0] IN1, IN2,
           wire CIN, // carry input, set it to HIGH to have no affect (in SUB)
    output wire [31:0] OUT,
           wire COUT, // carry output
           wire OVERFLOW
    );
    
    wire [32:0] tmp_OUT;
    
    assign tmp_OUT = IN1 + IN2 + CIN;
    assign OUT = tmp_OUT[31:0];
    assign COUT = tmp_OUT[32]; // carry
    assign OVERFLOW = ( IN1[31] &  IN2[31] & ~OUT[31]) | // both input is positive and result is negative
                      (~IN1[31] & ~IN2[31] &  OUT[31]);  // both input is negative and result is positive
    
endmodule
