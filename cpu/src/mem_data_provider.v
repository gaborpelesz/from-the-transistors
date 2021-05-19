`timescale 1ns / 1ps

module mem_data_provider(
    input  wire [31:0] data_in,
           wire        in_b_bus_en,
    output reg  [31:0] data_out0,
           wire [31:0] data_out1
    );
    
    always @ (in_b_bus_en, data_in)
        data_out0 = in_b_bus_en == 0 ? 32'bz : data_in;
    
    assign data_out1 = data_in;
    
endmodule
