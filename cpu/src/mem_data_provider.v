`timescale 1ns / 1ps

module mem_data_provider(
    input  wire [31:0] data_in,
           wire        path_sel,
    output wire [31:0] data_out0,
           wire [31:0] data_out1
    );
    
    // select between output 0 and 1
    assign data_out0 = path_sel ?   32'bz : data_in;
    assign data_out1 = path_sel ? data_in :   32'bz;
    
endmodule
