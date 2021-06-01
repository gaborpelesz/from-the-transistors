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
            // bootrom source
            {  mem[3],   mem[2],   mem[1],   mem[0]} <= 32'b11100011101000001101100000111000; // 0xe3a0d838 -> MOV R13, #56, 16
            {  mem[7],   mem[6],   mem[5],   mem[4]} <= 32'b11100011100011011100110000000100; // 0xe38dcc04 -> ORR R12, R13, #4, 24
            { mem[11],  mem[10],   mem[9],   mem[8]} <= 32'b11100001101000001011000000001100; // 0xe1a0b00c -> MOV R11, R12
            { mem[15],  mem[14],  mem[13],  mem[12]} <= 32'b11100011101000001010010011100011; // 0xe3a0a4e3 -> MOV R10, #227, 8
            { mem[19],  mem[18],  mem[17],  mem[16]} <= 32'b11100011100010101010100010100000; // 0xe38aa8a0 -> ORR R10, R10, #160, 16
            { mem[23],  mem[22],  mem[21],  mem[20]} <= 32'b11100011100010101010110011110000; // 0xe38aacf0 -> ORR R10, R10, #240, 24
            { mem[27],  mem[26],  mem[25],  mem[24]} <= 32'b11100001101000000000000000000001; // 0xe1a00001 -> MOV R0, R1
            { mem[31],  mem[30],  mem[29],  mem[28]} <= 32'b11100011101000000001000000000000; // 0xe3a01000 -> MOV R1, #0
            { mem[35],  mem[34],  mem[33],  mem[32]} <= 32'b11100011101000000010000000000000; // 0xe3a02000 -> MOV R2, #0
            { mem[39],  mem[38],  mem[37],  mem[36]} <= 32'b11100101100111010100000000000010; // 0xe59d4002 -> LDR R4, [R13, #+2]
            { mem[43],  mem[42],  mem[41],  mem[40]} <= 32'b11100011000101000000000000000100; // 0xe3140004 -> TST R4, #4
            { mem[47],  mem[46],  mem[45],  mem[44]} <= 32'b00001010111111111111111111111100; // 0x0afffffc -> BEQ #36
            { mem[51],  mem[50],  mem[49],  mem[48]} <= 32'b11100101100111010100000000000001; // 0xe59d4001 -> LDR R4, [R13, #+1]
            { mem[55],  mem[54],  mem[53],  mem[52]} <= 32'b11100101100011010100000000000011; // 0xe58d4003 -> STR R4, [R13, #+3]
            { mem[59],  mem[58],  mem[57],  mem[56]} <= 32'b11100001100001000001010000000001; // 0xe1841401 -> ORR R1, R4, R1, LSL #8
            { mem[63],  mem[62],  mem[61],  mem[60]} <= 32'b11100010100000100010000000000001; // 0xe2822001 -> ADD R2, R2, #1
            { mem[67],  mem[66],  mem[65],  mem[64]} <= 32'b11100011010100100000000000000100; // 0xe3520004 -> CMP R2, #4
            { mem[71],  mem[70],  mem[69],  mem[68]} <= 32'b00011010111111111111111111110110; // 0x1afffff6 -> BNE #36
            { mem[75],  mem[74],  mem[73],  mem[72]} <= 32'b11100011010100010000000000000000; // 0xe3510000 -> CMP R1, #0
            { mem[79],  mem[78],  mem[77],  mem[76]} <= 32'b00001010000000000000000000000001; // 0x0a000001 -> BEQ #88
            { mem[83],  mem[82],  mem[81],  mem[80]} <= 32'b11100100100010110001000000000100; // 0xe48b1004 -> STR R1, [R11], #+4
            { mem[87],  mem[86],  mem[85],  mem[84]} <= 32'b11101010111111111111111111101111; // 0xeaffffef -> B #24
            { mem[91],  mem[90],  mem[89],  mem[88]} <= 32'b11100100100010111010000000000100; // 0xe48ba004 -> STR R10, [R11], #+4
            { mem[95],  mem[94],  mem[93],  mem[92]} <= 32'b11100001101000001101000000001011; // 0xe1a0d00b -> MOV R13, R11
            { mem[99],  mem[98],  mem[97],  mem[96]} <= 32'b11100001101000001111000000001100; // 0xe1a0f00c -> MOV PC, R12
            
            for (i = 100; i < NUM_OF_BYTES; i = i + 4)
            begin
                // rest of the code is branching back at the beginning: MOV PC, #0
                {mem[i+3], mem[i+2], mem[i+1], mem[i]} <= 32'b11100011101000001111000000000000; // 0xe3a0f000 -> MOV PC, #0
            end
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

