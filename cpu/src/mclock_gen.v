`timescale 1ns / 1ps

module mclock_gen(
    input  wire        clk,
           wire        reset,
           wire [31:0] address,
           wire        stretch_mclk,
    output reg         mclk
    );
    
    reg [31:0] r_address = 32'b0;
    
    always @ (*)
    begin
        if (stretch_mclk)
        begin
        if (reset)
            mclk = clk;
        
        else if (r_address == address)
            mclk = clk;
        
        else
            mclk = 1;
        end
        else
        begin
            mclk = clk;
        end
    end
    
    // this will delay the MCLK by 1 clock cycle to enable data to be received on the mem bus
    always @ (posedge clk)
    begin
        if (reset)
            r_address <= 32'b0;

        else if (r_address != address)
            r_address <= address;
    end
    
endmodule
