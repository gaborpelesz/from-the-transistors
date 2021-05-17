`timescale 1ns / 1ps

/* Size adjustable RAM, with big-endian endianess */
module memory #(parameter NUM_OF_BYTES = 800)(
    input  wire        clk,
           wire [31:0] address,
           wire        write_en,
           wire [31:0] write_data,
    output reg  [31:0] read_data
    );
    
    reg [7:0] mem [0:NUM_OF_BYTES - 1]; // capable of storing 800 individual bytes by default
    
    // write on posedge
    always @ (posedge clk)
    begin
        if (write_en && address < NUM_OF_BYTES-3)
        begin
            mem[address]   <= write_data[7:0];
            mem[address+1] <= write_data[15:8];
            mem[address+2] <= write_data[23:16];
            mem[address+3] <= write_data[31:24];
        end
    end
    
    // read on negedge
    always @ (*)
    begin
        if (address < NUM_OF_BYTES-3)
        begin
            read_data = { mem[address+3], mem[address+2], mem[address+1], mem[address] };
        end else begin
            read_data = 32'bz;
        end
    end
    
endmodule
