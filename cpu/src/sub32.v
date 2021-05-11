`timescale 1ns / 1ps

module mSUB32(
    input  wire [31:0] IN1, IN2,
           wire CIN, // carry input, set it to HIGH to have no affect (in SUB)
    output wire [31:0] OUT,
           wire COUT, // carry output
           wire OVERFLOW
    );
    
    wire [32:0] tmp_OUT;
    
    assign tmp_OUT = IN1 - IN2 - ~CIN;
    assign OUT = tmp_OUT[31:0];

    assign COUT = ~ (~IN1[31] & OUT[31]);                       // borrow in the carry flag can only occur if the minuhend's
                                                                // sign is 0 and result's sign is 1 
    assign OVERFLOW = IN1[31] != IN2[31] && IN2[31] == OUT[31]; // overflow only occurs if the operands' signs are different
                                                                // and the subtrahend's sign and the result's sign are the same
    
endmodule
