`timescale 1ns / 1ps

/* Tri-state into the memory bus */
module write_data_register(
    input wire [31:0] B_bus,
          wire        data_out_en,
    output reg [31:0] data_out
    );
    
    always @ (*) data_out = data_out_en ? B_bus : 32'bz;
    
endmodule
