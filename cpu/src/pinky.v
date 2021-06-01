`timescale 1ns / 1ps

module pinky(
    input  wire        clk,
           wire        btn_center,
           wire        btn_right,
           wire        RxD,
           wire  [7:0] SW,
    output wire [15:0] LED,
           wire        TxD
    );
    
    wire        cpu_control_reset;
    wire [31:0] cpu_address_bus;
    wire        cpu_mem_write_en;
    wire [31:0] cpu_mem_write_data;
    wire [31:0] cpu_mem_read_data;
    wire [31:0] cpu_debug_output;
    wire        cpu_stretch_mclk;
    
    wire        uart_send_data_en;
    wire [31:0] uart_in_data;
    wire        uart_send_word;
    wire        uart_data_is_read;
    wire  [7:0] uart_out_data;
    wire  [2:0] uart_status; // { rx_data_valid, tx_active, tx_done }
    
    assign LED = cpu_debug_output[15:0];
    
    /* MEMORY MAPPER */
    wire [31:0] bootrom_address;
    wire [31:0] bootrom_read_data;
    wire [31:0] bram_read_data;
    wire [31:0] bram_address;
    wire [31:0] bram_write_data;
    wire        bram_write_en;
    wire        mmio_reset;
    wire [11:0] mmio_address;
    wire [31:0] mmio_write_data;
    wire        mmio_write_en;
    wire [31:0] mmio_read_data;
    memory_mapper memory_mapper_inst (.in_mem_reset(cpu_control_reset),
                                      .in_address(cpu_address_bus),
                                      .in_data(cpu_mem_write_data),
                                      .in_write_en(cpu_mem_write_en),
                                      .in_bootrom_read_data(bootrom_read_data),
                                      .in_nvm_read_data(32'b0),
                                      .in_mmio_read_data(mmio_read_data),
                                      .in_bram_read_data(bram_read_data),
                                      .out_read_data(cpu_mem_read_data),
                                      .out_bootrom_address(bootrom_address),
                                      .out_nvm_address(),
                                      .out_nvm_write_data(),
                                      .out_nvm_write_en(),
                                      .out_mmio_reset(mmio_reset),
                                      .out_mmio_address(mmio_address),
                                      .out_mmio_write_data(mmio_write_data),
                                      .out_mmio_write_en(mmio_write_en),
                                      .out_bram_address(bram_address),
                                      .out_bram_write_data(bram_write_data),
                                      .out_bram_write_en(bram_write_en));
                                      
    /* MMIO MAPPER */
    mmio_mapper mmio_mapper_inst (.in_uart_data(uart_out_data),
                                  .in_uart_status(uart_status),
                                  .out_uart_send_en(uart_send_data_en),
                                  .out_uart_data(uart_in_data),
                                  .out_uart_data_is_read(uart_data_is_read),
                                  .in_reset(mmio_reset),
                                  .in_address(mmio_address),
                                  .in_write_data(mmio_write_data),
                                  .in_write_en(mmio_write_en),
                                  .out_read_data(mmio_read_data));

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
    
    /* MEMORY CLOCK CYCLE GENERATOR --> if address has changed, will stall CLK until data appears on mem bus */
    wire mclk;
    mclock_gen mclock_gen_inst (.clk(clk),
                                .reset(btn_center),
                                .address(bram_address),
                                .stretch_mclk(cpu_stretch_mclk),
                                .mclk(mclk));
    
    /* CPU INIT */
    cpu cpu_inst(.clk(mclk),
                 .reset(btn_center),
                 .in_mem_read_data(cpu_mem_read_data),
                 .out_control_reset(cpu_control_reset),
                 .out_address_bus(cpu_address_bus),
                 .out_mem_write_en(cpu_mem_write_en),
                 .out_mem_write_data(cpu_mem_write_data),
                 .out_reg_debug_data(cpu_debug_output),
                 .stretch_mclk(cpu_stretch_mclk));
                 
/* ---- INPUT/OUTPUT ---- */
    
    /* UART */
    uart_controller uart_controller_inst (.clk(clk),
                                          .in_serial(RxD),
                                          .out_serial(TxD),
                                          .in_send_data_en(uart_send_data_en),
                                          .in_data(uart_in_data),
                                          .in_send_word(1'b0), // to be implemented
                                          .in_data_is_read(uart_data_is_read),
                                          .out_data(uart_out_data),
                                          .out_uart_status(uart_status));
    
endmodule
