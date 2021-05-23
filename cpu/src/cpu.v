`timescale 1ns / 1ps

module cpu(
    input  wire clk,
           wire reset,
           wire TxD,
           wire RxD,
           wire BTNR,
           wire [7:0] SW,
    output wire [15:0] LED
    );
    
    /* BUSES */
    wire [31:0] data_in_bus, data_out_bus; // memory in-out
    wire [31:0] address_bus;               // memory addressing
    wire [31:0] ALU_bus;
    wire [31:0] A_bus;
    wire [31:0] B_bus;
    wire [31:0] incrementer_bus;
    wire [31:0] PC_bus;
    wire [31:0] instruction_bus;
    
    /* Control signals */
    wire        c_mem_write_en;
    
    wire  [3:0] c_reg_read_A_sel;
    wire  [3:0] c_reg_read_B_sel;
    wire  [3:0] c_reg_read_C_sel;
    wire        c_reg_read_B_en;
    wire  [3:0] c_reg_write_sel;
    wire        c_reg_write_en;
    wire        c_reg_pc_write_en;
    wire        c_reg_cpsr_write_en;
    wire        c_reg_write_lr_en;
    wire        c_reset;
    
    wire  [1:0] c_address_reg_sel;
    wire        c_update_address;
    
    wire [31:0] c_barrel_shift_val;
    wire  [2:0] c_barrel_op_sel;
    
    wire  [3:0] c_alu_op_sel;
    
    wire        c_data_prov_b_bus_en;
    wire        c_data_out_en;
    
    wire [31:0] c_reg_shifter_value;
    
    wire  [3:0] reg_write_cpsr;
    wire  [3:0] reg_read_cpsr;
    /* end Control signals */
    
    
    
    /* DECODE AND LOGIC CONTROL MODULE INIT */
    logic_control logic_control_inst (.clk(clk),
                                      .reset(reset),
                                      .reg_shifter_value(c_reg_shifter_value),
                                      .mem_data_prov_instruction(instruction_bus),
                                      .in_cpsr(reg_read_cpsr),
                                      .mem_write_en(c_mem_write_en),
                                      .reg_read_A_sel(c_reg_read_A_sel),
                                      .reg_read_B_sel(c_reg_read_B_sel),
                                      .reg_read_C_sel(c_reg_read_C_sel),
                                      .reg_read_B_en(c_reg_read_B_en),
                                      .reg_write_sel(c_reg_write_sel),
                                      .reg_write_en(c_reg_write_en),
                                      .reg_pc_write_en(c_reg_pc_write_en),
                                      .reg_lr_write_en(c_reg_write_lr_en),
                                      .reg_cpsr_write_en(c_reg_cpsr_write_en),
                                      .address_reg_sel(c_address_reg_sel),
                                      .update_address(c_update_address),
                                      .barrel_shift_val(c_barrel_shift_val),
                                      .barrel_op_sel(c_barrel_op_sel),
                                      .alu_op_sel(c_alu_op_sel),
                                      .data_prov_b_bus_en(c_data_prov_b_bus_en),
                                      .data_out_en(c_data_out_en),
                                      .control_reset(c_reset),
                                      .out_immediate_value(B_bus));
    
    /* REGISTER BANK MODULE INIT */
    reg_bank reg_bank_inst (.clk(clk),
                            .read_A_select(c_reg_read_A_sel),    // control
                            .read_B_select(c_reg_read_B_sel),    // control
                            .read_C_select(c_reg_read_C_sel),
                            .read_B_en(c_reg_read_B_en),
                            .write_select(c_reg_write_sel),      // control
                            .write_en(c_reg_write_en),           // control
                            .write_data(ALU_bus),
                            .write_pc_en(c_reg_pc_write_en),     // control
                            .write_pc_data(incrementer_bus),
                            .write_lr_en(c_reg_write_lr_en),
                            .write_cpsr_data(reg_write_cpsr),
                            .write_cpsr_en(c_reg_cpsr_write_en), // control
                            .reset(c_reset),                     // control
                            .read_A_data(A_bus),
                            .read_B_data(B_bus),
                            .read_C_data(c_reg_shifter_value),
                            .read_pc_data(PC_bus),
                            .read_cpsr_data(reg_read_cpsr),
                            .debug_out(LED));
    
    /* ADDRESS REGISTER MODULE INIT */
    wire [31:0] address_reg_inc_bridge;
    address_register address_register_inst (.clk(clk),
                                            .reset(c_reset),
                                            .in0(ALU_bus),
                                            .in1(PC_bus),
                                            .in2(incrementer_bus),
                                            .in_select(c_address_reg_sel),                 // control
                                            .out_mem_address(address_bus),
                                            .out_inc_address(address_reg_inc_bridge),
                                            .update_address(c_update_address));

    /* ADDRESS INCREMENTER MODULE INIT */
    address_inc address_inc_inst (.in_address(address_reg_inc_bridge), 
                                  .out_address(incrementer_bus));

    /* BARREL SHIFTER MODULE INIT */
    wire [31:0] barrel_to_alu_bus;
    wire        barrel_to_alu_carry;
    barrel_shifter barrel_shifter_inst (.in_data(B_bus),
                                        .shift_value(c_barrel_shift_val),  // control
                                        .in_op_select(c_barrel_op_sel),    // control
                                        .in_carry(reg_read_cpsr[1]),                       // control, where does the carry comes from? we should read it from status register, but how?
                                        .out_shifted_data(barrel_to_alu_bus),
                                        .out_carry(barrel_to_alu_carry));

    /* ALU MODULE INIT */
    ALU32 alu32_inst (.in_op_select(c_alu_op_sel),      // control
                      .in_data0(A_bus),
                      .in_data1(barrel_to_alu_bus),
                      .in_carry(barrel_to_alu_carry),
                      .in_overflow(reg_read_cpsr[0]),   // control
                      .out_data(ALU_bus),
                      .out_neg(reg_write_cpsr[3]),       // control (CPSR update if S enabled)
                      .out_zero(reg_write_cpsr[2]),      // control (CPSR update if S enabled)
                      .out_carry(reg_write_cpsr[1]),     // control (CPSR update if S enabled)
                      .out_overflow(reg_write_cpsr[0])); // control (CPSR update if S enabled)
    
    /* MEMORY (BRAM) INIT */
    memory #(.NUM_OF_BYTES(1024)) ram_inst (.clk(clk),
                                           .address(address_bus),
                                           .write_en(c_mem_write_en),
                                           .write_data(B_bus),
                                           .reset(c_reset),
                                           .read_data(data_in_bus));
                                           
    /* MEMORY DATA PROVIDER INIT */
    // selects between B_bus and Control input
    mem_data_provider mem_data_provider_inst (.data_in(data_in_bus),
                                              .in_b_bus_en(c_data_prov_b_bus_en), // control
                                              .data_out0(B_bus),
                                              .data_out1(instruction_bus));
                                              
    /* WRITE DATA REGISTER INIT */
    write_data_register write_data_register_inst (.B_bus(B_bus),
                                                  .data_out_en(c_data_out_en),
                                                  .data_out(data_out_bus));
    
endmodule
