`timescale 1ns / 1ps

/* Adjustable RAM, using Big-endian */
module memory #(parameter NUM_OF_BYTES = 800)(
    input  wire        CLK,
           wire [31:0] address,
           wire        read_en,
           wire        write_en,
           wire [31:0] write_data,
    output reg  [31:0] read_data
    );
    
    reg [7:0] mem [0:NUM_OF_BYTES - 1]; // capable of storing 800 individual bytes 
    
    always @ (negedge CLK)
    begin
        if (read_en && address < NUM_OF_BYTES-3)
        begin
            read_data <= { mem[address+3], mem[address+2], mem[address+1], mem[address] };
        end else begin
            read_data <= 32'bz;
        end
        
        if (write_en && address < NUM_OF_BYTES-3)
        begin
            mem[address]   <= write_data[7:0];
            mem[address+1] <= write_data[15:8];
            mem[address+2] <= write_data[23:16];
            mem[address+3] <= write_data[31:24];
        end
    end
    
endmodule
