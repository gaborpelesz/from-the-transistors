`timescale 1ns / 1ps

/* Size adjustable RAM, that will infer BRAM */
module bram_spram #(parameter WIDTH = 32, parameter DEPTH = 1024)(
    input  wire                   clk,
           wire [31:0]            address,
           wire                   write_en,
           wire [WIDTH-1:0]       write_data,
    output reg  [WIDTH-1:0]       read_data
    );
    
    reg [WIDTH-1:0] mem [DEPTH-1:0];

    // write on posedge
    always @ (posedge clk)
    begin
        if (write_en)
        begin
            mem[address] <= write_data;
            read_data    <= write_data;
        end
        else
            read_data <= mem[address];
    end
    
endmodule
