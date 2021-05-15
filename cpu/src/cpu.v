`timescale 1ns / 1ps

module cpu(
    input wire clk
    );
    
    /* BUSES */
    wire [31:0] data_in_bus, data_out_bus;    // memory in-out
    wire [31:0] address_bus; // memory addressing
    wire [31:0] ALU_bus;
    reg  [31:0] A_bus;
    reg  [31:0] B_bus;
    wire [31:0] incrementer_bus;
    wire [31:0] PC_bus;
    
    /* Control signals */
    wire c_mem_read_en;
    wire c_mem_write_en;
    
    /* Creating a dynamic Memory CLK
       It runs the memory accesses and internal operations
       It can stretch indefinitely to allow working with slow peripherals or memory */
    localparam N_Cycle = 2'b00,
               S_Cycle = 2'b01,
               I_Cycle = 2'b10,
               C_Cycle = 2'b11;
    reg [1:0] nMREQ_SEQ = S_Cycle; // MSB=nMREQ, LSB=SEQ
    reg MCLK;
    // the problem with the below always block is that MCLK won't be assigned to the
    // block, rather it's value will be updated when the negedge of the CLK happens.
    // This leads us to an MCLK which is always 0, because it only changes when the CLK gets 0.
    // It is also not synthesisable because we can't usse the clock signal in expression.
    always @ (negedge clk)
    begin
        /* Non-sequential cycle, stretching MCLK */
        if (nMREQ_SEQ == N_Cycle)
        begin
            MCLK <= 0;
            
            if (data_in_bus != 32'bz)
            begin
                nMREQ_SEQ <= S_Cycle;
            end
        end
        
        /* Sequential cycle, MCLK is CLK */
        else if (nMREQ_SEQ == S_Cycle)
        begin
            MCLK <= clk;
        end
        
        /* Default behavior */
        else
        begin
            nMREQ_SEQ <= S_Cycle;
            MCLK <= clk;
        end
    end // end MCLK
    
    /* REGISTER BANK MODULE INIT */
    wire [3:0] reg_rs, reg_rt, reg_wr, reg_w_en;
    wire [31:0] reg_wd;
    wire [31:0] reg_rsd, reg_rtd;
    reg_bank reg_bank_inst (.clk(clk),
                            .read_A_select(),
                            .read_B_select(),
                            .write_select(),
                            .write_en(),
                            .write_data(ALU_bus),
                            .write_pc_en(),
                            .write_pc_data(incrementer_bus),
                            .read_A_data(A_bus),
                            .read_B_data(B_bus),
                            .read_pc_data(PC_bus));
    
    /* ADDRESS REGISTER MODULE INIT */
    wire [31:0] address_reg_inc_bridge;
    address_register address_register_inst (.clk(clk),
                                            .in0(ALU_bus),
                                            .in1(PC_bus),
                                            .in2(incrementer_bus),
                                            .in_select(),
                                            .perform_address_update(),
                                            .out_mem_address(address_bus),
                                            .out_inc_address(address_reg_inc_bridge));

    /* ADDRESS INCREMENTER MODULE INIT */
    address_inc address_inc_inst (.in_address(address_reg_inc_bridge), 
                                  .out_address(incrementer_bus));

    /* BARREL SHIFTER MODULE INIT */
    wire [31:0] barrel_to_alu_bus;
    wire        barrel_to_alu_carry;
    barrel_shifter barrel_shifter_inst (.in_data(B_bus),
                                        .shift_value(),  // control
                                        .in_op_select(), // control
                                        .in_carry(),     // control
                                        .out_shifted_data(barrel_to_alu_bus),
                                        .out_carry(barrel_to_alu_carry));

    /* ALU MODULE INIT */
    ALU32 alu32_inst (.in_op_select(),  // control
                      .in_data0(A_bus),
                      .in_data1(barrel_to_alu_bus),
                      .in_carry(barrel_to_alu_carry),
                      .in_neg(),        // control
                      .in_zero(),       // control
                      .in_overflow(),   // control
                      .out_data(ALU_bus),
                      .out_carry(),     // control (CPSR update if S enabled)
                      .out_neg(),       // control (CPSR update if S enabled)
                      .out_zero(),      // control (CPSR update if S enabled)
                      .out_overflow()); // control (CPSR update if S enabled)
    
    /* MEMORY (BRAM) INIT */
    memory #(.NUM_OF_BYTES(800)) ram_inst (.address(address_bus),
                                           .read_en(c_mem_read_en),
                                           .write_en(c_mem_write_en),
                                           .write_data(data_out_bus),
                                           .read_data(data_in_bus));
    
    /* Pipeline registers */
    reg fetch_enable = 1'b1;
    reg decode_enable = 1'b1;
    reg execute_enable = 1'b1;
    // fetch-decode
    reg [31:0] fd_instruction;
    // decode-execute
    
    /* CONTROL */
    always @ (posedge MCLK)
    begin
        /* Fetch stage */
        if (fetch_enable)
        begin
            // send out read request
            // wait for data and save on MCLK
            // get current instruction from RAM pointed by PC
            // increment PC
        end
        
        /* Decode stage */
        if (decode_enable)
        begin
        
        end
        
        /* Execute stage */
        if (execute_enable)
        begin
        end
    end
    
    always @ (negedge MCLK)
    begin
        
    end
    
endmodule
