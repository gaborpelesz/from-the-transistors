`timescale 1ns / 1ps

module pinky(
    input  wire        clk,
           wire        btn_center,
           wire        btn_right,
           wire        TxD,
           wire        RxD,
           wire  [7:0] SW,
    output wire [15:0] LED
    );
    
    wire master_clk = LED[0] == 0 ? clk : btn_right; // enable stepping with right button when switch 0 is turned on
    
    wire        cpu_control_reset;
    wire [31:0] cpu_address_bus;
    wire        cpu_mem_write_en;
    wire [31:0] cpu_mem_write_data;
    wire [31:0] cpu_mem_read_data;
    wire [31:0] cpu_debug_output;
    
    assign LED = cpu_debug_output[15:0];
    
    
    /* MEMORY MAPPER */
    wire [31:0] bootrom_address;
    wire [31:0] bootrom_read_data;
    wire [31:0] bram_read_data;
    wire [31:0] bram_address;
    wire [31:0] bram_write_data;
    wire        bram_write_en;
    memory_mapper memory_mapper_inst (.in_mem_reset(cpu_control_reset),
                                      .in_address(cpu_address_bus),
                                      .in_data(cpu_mem_write_data),
                                      .in_write_en(cpu_mem_write_en),
                                      .in_bootrom_read_data(bootrom_read_data),
                                      .in_nvm_read_data(32'b0),
                                      .in_mmio_read_data(32'b0),
                                      .in_bram_read_data(bram_read_data),
                                      .out_read_data(cpu_mem_read_data),
                                      .out_bootrom_address(bootrom_address),
                                      .out_nvm_address(),
                                      .out_nvm_write_data(),
                                      .out_nvm_write_en(),
                                      .out_mmio_reset(),
                                      .out_mmio_address(),
                                      .out_mmio_write_data(),
                                      .out_mmio_write_en(),
                                      .out_bram_address(bram_address),
                                      .out_bram_write_data(bram_write_data),
                                      .out_bram_write_en(bram_write_en));
    
    /* BOOTROM INIT (1KB) */
    fast_memory #(.NUM_OF_BYTES(1024)) bootrom_inst (.clk(clk),
                                                     .mem_reset(cpu_control_reset),   // read-only
                                                     .address(bootrom_address),
                                                     .write_en(1'b0),    // read-only
                                                     .write_data(32'b0), // read-only
                                                     .read_data(bootrom_read_data));
    
    /* BRAM INIT (100KB) */
    bram_spram #(.WIDTH(32), .DEPTH(1024*25)) bram_inst (.clk(clk),
                                              .address(bram_address),
                                              .write_en(bram_write_en),
                                              .write_data(bram_write_data),
                                              .read_data(bram_read_data));
    
    /* CPU INIT */
    cpu cpu_inst(.clk(clk),
                 .reset(btn_center),
                 .in_mem_read_data(cpu_mem_read_data),
                 .out_control_reset(cpu_control_reset),
                 .out_address_bus(cpu_address_bus),
                 .out_mem_write_en(cpu_mem_write_en),
                 .out_mem_write_data(cpu_mem_write_data),
                 .out_reg_debug_data(cpu_debug_output));
    
endmodule
