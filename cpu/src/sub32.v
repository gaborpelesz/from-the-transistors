`timescale 1ns / 1ps

module sub32(
    input  wire [31:0] in_data0, 
           wire [31:0] in_data1,
           wire        in_carry, // carry input, set it to HIGH to have no affect (in SUB)
    output wire [31:0] out_data,
           wire        out_carry, // carry output
           wire        out_overflow
    );
    
    wire [32:0] result;
    
    wire negated_carry    = ~in_carry; // if we do not negate the carry in a separate wire
                                       // the negation will be incorrect because of some Vivado magic
    assign result       = in_data0 - in_data1 - negated_carry;
    assign out_data     = result[31:0];

    assign out_carry    = ~ (~in_data0[31] & out_data[31]);  // borrow in the carry flag can only occur if the minuhend's
                                                             // sign is 0 and result's sign is 1
    assign out_overflow = in_data0[31] != in_data1[31] &&    // overflow only occurs if the operands' signs are different
                          in_data1[31] == out_data[31];      // and the subtrahend's sign and the result's sign are the same
                          
endmodule
