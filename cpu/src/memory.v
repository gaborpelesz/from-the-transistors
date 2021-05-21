`timescale 1ns / 1ps

/* Size adjustable RAM, with big-endian endianess */
module memory #(parameter NUM_OF_BYTES = 800)(
    input  wire        clk,
           wire [31:0] address,
           wire        write_en,
           wire [31:0] write_data,
           wire        reset,
    output reg  [31:0] read_data
    );
    
    reg [7:0] mem [0:NUM_OF_BYTES - 1]; // capable of storing 800 individual bytes by default
    
    integer i;

    // write on posedge
    always @ (posedge clk)
    begin
        if (reset)
        begin
            /*
            // init 0x0000 with:
            //  - MOV R14, #5
            //                                      cond mod  opc S  Rn   Rd  rotimm  immed_8                            
            {mem[3], mem[2], mem[1], mem[0]} <= 32'b0000_001_1101_0_0000_1110__0000__00000101;
                                                  //0000_0011_1010_0000_0101__0000__0000_0101;
            */
            
            {mem[3], mem[2], mem[1], mem[0]}     <= 32'b11100011101000000000000000000101; // MOV R0, #5
            {mem[7], mem[6], mem[5], mem[4]}     <= 32'b11100011101000000001000000001111; // MOV R1, #15
            {mem[11], mem[10], mem[9], mem[8]}   <= 32'b11100000100000000000000000000001; // ADD R0, R0, R1  ;R0==20
            {mem[15], mem[14], mem[13], mem[12]} <= 32'b11100001101000000101000000000001; // MOV R5, R1      ;R5==15
            {mem[19], mem[18], mem[17], mem[16]} <= 32'b11100010100000000000000000000001; // ADD R0, R0, #1  ;++R0==21
            {mem[23], mem[22], mem[21], mem[20]} <= 32'b11100000010000001110000000000101; // SUB R14, R0, R5 ;R14==6
                                                 
                                                 
            for (i = 24; i < NUM_OF_BYTES; i = i + 4)
            begin
                // because no branch, memory should be full of NOP
                {mem[i+3], mem[i+2], mem[i+1], mem[i]} <= 32'b11100001101000000000000000000000;
                //mem[i] <= 8'b0;
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
        begin
            read_data = { mem[address+3], mem[address+2], mem[address+1], mem[address] };
        end else begin
            read_data = 32'bz;
        end
    end
    
endmodule
