`timescale 1ns / 1ps

/* Tri-state into the memory bus */
module write_data_register(
    input wire        clk,
          wire [31:0] B_bus,
          wire  [1:0] data_out_sel,
          wire        reg_write_en,
    output reg [31:0] data_out
    );
    
    localparam DATA_OUT_HIGH_IMP     = 2'b00,
               DATA_OUT_PASS_THROUGH = 2'b01,
               DATA_OUT_LATCHED      = 2'b10;
    
    reg [31:0] data_out_reg;
    
    always @ (posedge clk)
        if (reg_write_en) data_out_reg <= B_bus;
    
    always @ (*)
        case (data_out_sel)
            DATA_OUT_HIGH_IMP     : data_out = 32'bz;
            DATA_OUT_PASS_THROUGH : data_out = B_bus;
            DATA_OUT_LATCHED      : data_out = data_out_reg;
            default               : data_out = 32'bz; 
        endcase
    
endmodule
