`timescale 1ns / 1ps

/* Tri-state into the memory bus */
module write_data_register(
    input wire [31:0] B_bus,
          wire        data_out_en,
    output reg [31:0] data_out
    );
    
    always @ (*)
    begin
        if (data_out_en)
            data_out = B_bus;
        else
            data_out = 32'bz;
    end
    
endmodule
