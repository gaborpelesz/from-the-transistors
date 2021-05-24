`timescale 1ns / 1ps

module memory_mapper(
    input  wire        in_mem_reset,
    
           wire [31:0] in_address,
           wire [31:0] in_data,
           wire        in_write_en,
           
           wire [31:0] in_bootrom_read_data,
           wire [31:0] in_nvm_read_data,
           wire [31:0] in_mmio_read_data,
           wire [31:0] in_bram_read_data,
           
   output  reg  [31:0] out_read_data, // wire back to cpu
   
           reg  [31:0] out_bootrom_address, // rom is not writeable, hence no write outputs for this

           reg  [31:0] out_nvm_address,
           reg  [31:0] out_nvm_write_data,
           reg  [31:0] out_nvm_write_en,
           
           reg         out_mmio_reset,
           reg  [31:0] out_mmio_address,
           reg  [31:0] out_mmio_write_data,
           reg  [31:0] out_mmio_write_en,
           
           reg  [31:0] out_bram_address,
           reg  [31:0] out_bram_write_data,
           reg  [31:0] out_bram_write_en
    );
    
    localparam ENABLE = 1'b1, DISABLE = 1'b0;
    
    always @ (*)
    begin
        out_mmio_reset = in_mem_reset;
    
        // Bootrom: 0x0000_0000 <--> 0x0000_03FF => 1024B
        if (in_address < 32'h0000_0400)
        begin
            out_bootrom_address  = in_address;
            
            out_nvm_address     = 32'b0;
            out_nvm_write_data  = 32'b0;
            out_nvm_write_en    = DISABLE;

            out_mmio_address    = 32'b0;
            out_mmio_write_data = 32'b0;
            out_mmio_write_en   = DISABLE;
            
            out_bram_address    = 32'b0;
            out_bram_write_data = 32'b0;
            out_bram_write_en   = DISABLE;
            
            out_read_data       = in_bootrom_read_data;
        end
        
        // NVM data memory 0x0000_0400 <--> 0x0037_FFFF => 3583KB = ~3.499MB
        else if (in_address < 32'h0038_0000)
        begin
            // currently unavailable...
            out_bootrom_address  = 32'b0;
            
            out_nvm_address     = 32'b0;
            out_nvm_write_data  = 32'b0;
            out_nvm_write_en    =  1'b0;
            
            out_mmio_address    = 32'b0;
            out_mmio_write_data = 32'b0;
            out_mmio_write_en   =  1'b0;
            
            out_bram_address    = 32'b0;
            out_bram_write_data = 32'b0;
            out_bram_write_en   =  1'b0;
            
            out_read_data       = 32'bx;
        end
        
        // Mapped I/O
        else if (in_address < 32'h0038_0400)
        begin
            // what I/O ports do we have? Map them here...
            
            // currently unavailable...
            out_bootrom_address  = 32'b0;
            
            out_nvm_address     = 32'b0;
            out_nvm_write_data  = 32'b0;
            out_nvm_write_en    =  1'b0;
            
            out_mmio_address    = 32'b0;
            out_mmio_write_data = 32'b0;
            out_mmio_write_en   =  1'b0;
            
            out_bram_address    = 32'b0;
            out_bram_write_data = 32'b0;
            out_bram_write_en   =  1'b0;
            
            out_read_data       = 32'bx;
        end
        
        // BRAM 0x0038_0400 <--> 0x0039_93FF => 100KB
        else if (in_address < 32'h0039_9400)
        begin
            out_bootrom_address  = 32'b0;
            
            out_nvm_address     = 32'b0;
            out_nvm_write_data  = 32'b0;
            out_nvm_write_en    =  1'b0;
            
            out_mmio_address    = 32'b0;
            out_mmio_write_data = 32'b0;
            out_mmio_write_en   =  1'b0;
            
            out_bram_address     = in_address - 32'h0038_0400;
            out_bram_write_data  = in_data;
            out_bram_write_en    = in_write_en;

            out_read_data = in_bram_read_data;
        end
        
        // Reserved for future use 0x0039_9400 <--> 0xFFFF_FFFF
        else
        begin
            out_bootrom_address  = 32'b0;
            
            out_nvm_address     = 32'b0;
            out_nvm_write_data  = 32'b0;
            out_nvm_write_en    =  1'b0;
            
            out_mmio_address    = 32'b0;
            out_mmio_write_data = 32'b0;
            out_mmio_write_en   =  1'b0;
            
            out_bram_address    = 32'b0;
            out_bram_write_data = 32'b0;
            out_bram_write_en   =  1'b0;
            
            out_read_data       = 32'bx;
        end
    end
    
endmodule
