`timescale 1ns / 1ps

module address_inc(
    input  wire [31:0] in_address,
    output wire [31:0] out_address
    );
    
    assign out_address = in_address + 4;
    
endmodule
