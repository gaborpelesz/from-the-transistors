`timescale 1ns / 1ps

/* Size adjustable fast RAM, with big-endian endianess 
    - it reads out 4 bytes at once (1 cycle for 32bit data)
    - read lines can always be read
    - perfect for small ROM and/or MMIO (~1KB)
    - but it will NOT infer BRAM!
*/
module fast_memory #(parameter NUM_OF_BYTES = 1024)(
    input  wire        clk,
           wire        mem_reset,
           wire [31:0] address,
           wire        write_en,
           wire [31:0] write_data,
    output reg  [31:0] read_data
    );
    
    reg [7:0] mem [0:NUM_OF_BYTES - 1];
    
    integer i;

    // write on posedge
    always @ (posedge clk)
    begin
        if (mem_reset)
        begin
            // testing store
            { mem[3],  mem[2],  mem[1],  mem[0]} <= 32'b11100011101000000000000000010100; // MOV R0, #20
            { mem[7],  mem[6],  mem[5],  mem[4]} <= 32'b11100011101000001101100000111000; // MOV R13, #56, 16
            {mem[11], mem[10],  mem[9],  mem[8]} <= 32'b11100011100011011101010000000100; // ORR R13, R13, #4, 8
            {mem[15], mem[14], mem[13], mem[12]} <= 32'b11100110100011010000010000000000; // STR R0, [R13], +R0, LSL #8
            {mem[19], mem[18], mem[17], mem[16]} <= 32'b11100111001111010001010000000000; // LDR R1, [R13, -R0, LSL #8]!
            {mem[23], mem[22], mem[21], mem[20]} <= 32'b11100001101000000000000000000000; // MOV R0, R0
            {mem[27], mem[26], mem[25], mem[24]} <= 32'b11101010111111111111111111111101; // B #20

            //for (i = 32; i < NUM_OF_BYTES; i = i + 4)
            //begin
                // because no branch, memory should be full of NOP
                //{mem[i+3], mem[i+2], mem[i+1], mem[i]} <= 32'b11100001101000000000000000000000;
                //mem[i] <= 8'b0;
            //end
        end
        else if (write_en && address < NUM_OF_BYTES-3)
        begin
            mem[address]   <= write_data[7:0];
            mem[address+1] <= write_data[15:8];
            mem[address+2] <= write_data[23:16];
            mem[address+3] <= write_data[31:24];
        end
    end
    
    always @ (*)
    begin
        if (address < NUM_OF_BYTES-3)
            read_data = { mem[address+3], mem[address+2], mem[address+1], mem[address] };
        else
            read_data = 32'bx;
    end
endmodule

