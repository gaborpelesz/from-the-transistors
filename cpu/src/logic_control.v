`timescale 1ns / 1ps

/* instruction decoder and logic control */
module logic_control(
    input  wire        clk,
           wire        reset,
           wire [31:0] reg_read_C_bus,
           wire [31:0] mem_data_prov_instruction,
           wire  [3:0] in_cpsr,
    output reg         mem_write_en,
           reg   [3:0] reg_read_A_sel,
           reg   [3:0] reg_read_B_sel,
           wire  [3:0] reg_read_C_sel,
           reg         reg_read_B_en,
           reg   [3:0] reg_write_sel,
           reg         reg_write_en,
           reg         reg_pc_write_en,
           reg         reg_lr_write_en,
           reg         reg_cpsr_write_en,
           reg   [1:0] address_reg_sel,
           reg         update_address,
           reg  [31:0] barrel_shift_val,
           reg   [2:0] barrel_op_sel,
           reg   [3:0] alu_op_sel,
           reg         data_prov_b_bus_en,
           reg   [1:0] data_out_sel,
           reg         data_out_reg_write_en,
           reg         control_reset,
           wire [31:0] out_immediate_value,
           reg         stretch_mclk
    );
    
    localparam DECODE_DATA_PROC_IMM_SHIFT  = 4'b0000,
               DECODE_DATA_PROC_REG_SHIFT  = 4'b0001,
               DECODE_DATA_PROC_IMM_1      = 4'b0010,
               DECODE_DATA_PROC_IMM_2      = 4'b0011,
               DECODE_LOAD_STORE_IMM_OFF_1 = 4'b0100,
               DECODE_LOAD_STORE_IMM_OFF_2 = 4'b0101,
               DECODE_LOAD_STORE_REG_OFF   = 4'b0110,
               DECODE_BRANCH_AND_BL_1      = 4'b1010,
               DECODE_BRANCH_AND_BL_2      = 4'b1011;
    
    localparam ADDRESS_SELECT_ALU = 2'b00,
               ADDRESS_SELECT_PC  = 2'b01,
               ADDRESS_SELECT_INC = 2'b10;
    
    localparam R0  = 4'd00, R1  = 4'd01, R2  = 4'd02, R3  = 4'd03,
               R4  = 4'd04, R5  = 4'd05, R6  = 4'd06, R7  = 4'd07,
               R8  = 4'd08, R9  = 4'd09, R10 = 4'd10, R11 = 4'd11,
               R12 = 4'd12, R13 = 4'd13, R14 = 4'd14, R15 = 4'd15,
               LR  = 4'd14, PC  = 4'd15;
               
    localparam BARREL_OP_LSL = 2'b00, // logical shift left
               BARREL_OP_LSR = 2'b01, // logical shift right
               BARREL_OP_ASR = 2'b10, // arithmetic shift right
               BARREL_OP_ROR = 2'b11, // rotate right
               BARREL_OP_RRX = 3'b100; // rotate right with extend
               
    localparam ALU_OP_AND = 4'b0000, // bitwise AND
               ALU_OP_EOR = 4'b0001, // bitwise XOR
               ALU_OP_SUB = 4'b0010, // subtract
               ALU_OP_RSB = 4'b0011, // reverse subtract; note: to negate val in <Rm> use " RSBS <Rd>, <Rm>, #0 "
               ALU_OP_ADD = 4'b0100, // add
               ALU_OP_ADC = 4'b0101, // add with carry
               ALU_OP_SBC = 4'b0110, // subtract with carry
               ALU_OP_RSC = 4'b0111, // reverse subtract with carry
               ALU_OP_TST = 4'b1000, // test
               ALU_OP_TEQ = 4'b1001, // test equivalence
               ALU_OP_CMP = 4'b1010, // compare
               ALU_OP_CMN = 4'b1011, // compare negated
               ALU_OP_ORR = 4'b1100, // or
               ALU_OP_MOV = 4'b1101, // move, only important when updating CPSR
               ALU_OP_BIC = 4'b1110, // bit clear
               ALU_OP_MVN = 4'b1111; // move not, only important when updating CPSR
    
    localparam DISABLE = 1'b0,
               ENABLE  = 1'b1;
    
    localparam DATA_PROV_MEM_READ    = 1'b0,
               DATA_PROV_INSTRUCTION = 1'b1;
               
    localparam DATA_OUT_HIGH_IMP     = 2'b00,
               DATA_OUT_PASS_THROUGH = 2'b01,
               DATA_OUT_LATCHED      = 2'b10;
    
    localparam PIPELINE_RESET_1       = 3'b000,
               PIPELINE_RESET_2       = 3'b001,
               PIPELINE_FETCH         = 3'b010,
               PIPELINE_DECODE        = 3'b011,
               PIPELINE_EXECUTE       = 3'b100,
               PIPELINE_EXECUTE_STORE = 3'b101,
               PIPELINE_EXECUTE_LOAD  = 3'b110;
               //PIPELINE_STALL   = 3'b111;
    
    // fetch-decode pipeline registers
    reg [31:0] fd_instruction = 32'b11100001101000000000000000000000; // initialize to NOP (MOV R0, R0)
    
    wire [3:0] decoded_instruction_type; 
    assign decoded_instruction_type = {fd_instruction[27:25], fd_instruction[4]}; // ARM ARM p110   
    
    // decode-execute pipeline registers
    wire         de_instruction_ldr_str;
    wire   [3:0] de_reg_read_A_sel;
    wire   [3:0] de_reg_read_B_sel;
    wire   [3:0] de_reg_write_sel;
    wire   [2:0] de_barrel_op_sel;
    wire   [3:0] de_alu_op_sel;
    wire  [31:0] de_barrel_shift_val;
    wire  [31:0] de_immediate_value;
    wire         de_reg_read_B_en;
    wire         de_data_prov_b_bus_en;
    wire         de_imm_output_en;
    wire         de_reg_write_en;
    wire         de_reg_pc_write_en;
    wire         de_reg_lr_write_en;
    wire         de_reg_cpsr_write_en;
    wire   [1:0] de_data_out_sel;
    wire         de_data_out_reg_write_en;
    wire         de_mem_write_en;
    wire         de_addreg_update;
    wire   [1:0] de_addreg_sel;

    // multi cycle execute pipeline regs
    reg          ex_mem_write_en;
    reg    [1:0] ex_data_out_sel;
    reg          ex_data_out_reg_write_en;

    /* Immediate data provider module init */
    reg         imm_output_en   = DISABLE;
    reg  [31:0] immediate_value = 32'b0;
    imm_data_provider imm_data_provider_inst (.in_immediate(immediate_value),
                                              .in_output_en(imm_output_en),
                                              .out_b_bus(out_immediate_value));
    
    /* INSTRUCTION DECODER MODULE */
    instruction_decoder instruction_decoder_inst (fd_instruction,
                                                  reg_read_C_bus,
                                                  in_cpsr,
                                                  de_instruction_ldr_str,
                                                  de_reg_read_A_sel,
                                                  de_reg_read_B_sel,
                                                  reg_read_C_sel,
                                                  de_reg_write_sel,
                                                  de_barrel_op_sel,
                                                  de_alu_op_sel,
                                                  de_barrel_shift_val,
                                                  de_immediate_value,
                                                  de_reg_read_B_en,
                                                  de_data_prov_b_bus_en,
                                                  de_imm_output_en,
                                                  de_reg_write_en,
                                                  de_reg_pc_write_en,
                                                  de_reg_lr_write_en,
                                                  de_reg_cpsr_write_en,
                                                  de_data_out_sel,
                                                  de_data_out_reg_write_en,
                                                  de_mem_write_en,
                                                  de_addreg_update,
                                                  de_addreg_sel);
    
    /* Pipeline state machine */
    reg [2:0] pipeline_current_state = PIPELINE_RESET_1;
    reg [2:0] pipeline_next_state    = PIPELINE_FETCH;
    always @ (posedge clk) // or negedge, experiment with both
    begin
        if (reset)
            pipeline_current_state <= PIPELINE_RESET_1;
        else
            pipeline_current_state <= pipeline_next_state;
    end
    
    always @ (*) 
    begin
        // RESET -> FETCH -> DECODE -> EXECUTE ----> EXECUTE ----> EXECUTE ___ 
        //           _^_                         \             \              \
        //            \__________________________/_____________/______________/
        //
        case (pipeline_current_state)
            PIPELINE_RESET_1       : pipeline_next_state = PIPELINE_RESET_2;
            PIPELINE_RESET_2       : pipeline_next_state = PIPELINE_FETCH;
            PIPELINE_FETCH         : pipeline_next_state = PIPELINE_DECODE;
            PIPELINE_DECODE        : pipeline_next_state = de_instruction_ldr_str == 0 ? PIPELINE_EXECUTE : PIPELINE_EXECUTE_STORE ;
            PIPELINE_EXECUTE_STORE : pipeline_next_state = ex_mem_write_en == 0 ? PIPELINE_EXECUTE_LOAD : PIPELINE_EXECUTE;
            PIPELINE_EXECUTE_LOAD  : pipeline_next_state = PIPELINE_EXECUTE;
            PIPELINE_EXECUTE       : pipeline_next_state = PIPELINE_FETCH; // either de_data_prov_b_bus_en or mem_write_en or data_out_en is on, go to execute_2 instead
            default                : pipeline_next_state = PIPELINE_RESET_1;
        endcase
    end
    
    /* NEW PIPELINE - only on posedge */
    always @ (posedge clk)
    begin
        if (pipeline_current_state == PIPELINE_RESET_1)
            begin
                // signal other components to reset
                control_reset       <= ENABLE;
            
                // reset every output
                mem_write_en          <= DISABLE;
                reg_read_A_sel        <= R0;
                reg_read_B_sel        <= R0;
                reg_read_B_en         <= ENABLE;
                reg_write_sel         <= R0;
                reg_write_en          <= DISABLE;
                reg_pc_write_en       <= DISABLE;
                reg_lr_write_en       <= DISABLE;
                reg_cpsr_write_en     <= DISABLE;
                address_reg_sel       <= ADDRESS_SELECT_PC;
                update_address        <= ENABLE;
                barrel_shift_val      <= 32'b0;
                barrel_op_sel         <= BARREL_OP_LSL;
                alu_op_sel            <= ALU_OP_MOV;
                data_prov_b_bus_en    <= DISABLE;
                data_out_sel          <= DATA_OUT_HIGH_IMP;
                data_out_reg_write_en <= DISABLE;
                immediate_value       <= 32'b0;
                imm_output_en         <= DISABLE;
                
                // reset every pipeline register
                fd_instruction = 32'b11100001101000000000000000000000; // init NOP
            end

        else if (pipeline_current_state == PIPELINE_RESET_2)
            begin
                control_reset   <= DISABLE;
                
                // enable incrementing the PC
                update_address  <= ENABLE;
                reg_pc_write_en <= ENABLE;
                address_reg_sel <= ADDRESS_SELECT_INC;
            end
        
        else if (pipeline_current_state == PIPELINE_FETCH)
            begin
                // simply skip this block when pipelining
                //address_reg_sel    <= de_addreg_sel; // don't do this when pipelining
                update_address     <= DISABLE;       // don't do this when pipelining
                reg_pc_write_en    <= DISABLE;       // don't do this when pipelining
                reg_lr_write_en    <= DISABLE;
                stretch_mclk       <= DISABLE;
                // ------------------------------------------------------------------------
            
                // fetch instruction
                fd_instruction     <= mem_data_prov_instruction;
                
                // disable mem write, and prevent to put data on MEM write bus
                mem_write_en          <= DISABLE;
                data_out_sel          <= DATA_OUT_HIGH_IMP;
                data_out_reg_write_en <= DISABLE;
                
                // prevent CPSR write and any other register write to happen (except R15)
                reg_cpsr_write_en <= DISABLE;
                reg_write_en      <= DISABLE;
            end
        
        else if (pipeline_current_state == PIPELINE_DECODE)
            begin
                ex_mem_write_en <= de_mem_write_en;
                ex_data_out_sel <= de_data_out_sel;
                data_out_reg_write_en <= de_data_out_reg_write_en;
                
                address_reg_sel <= de_addreg_sel;
                update_address  <= de_addreg_update;
                reg_pc_write_en <= de_reg_pc_write_en;
                
                reg_read_A_sel     <= de_reg_read_A_sel;
                reg_read_B_sel     <= de_reg_read_B_sel;
                reg_write_sel      <= de_reg_write_sel;
                immediate_value    <= de_immediate_value;
                data_prov_b_bus_en <= de_data_prov_b_bus_en;
                
                if (de_data_out_reg_write_en == ENABLE && de_data_out_sel == DATA_OUT_LATCHED)
                begin
                    barrel_shift_val   <= 32'hFFFF_FFFF;
                    barrel_op_sel      <= BARREL_OP_LSL;
                end
                else
                begin
                    barrel_shift_val   <= de_barrel_shift_val;
                    barrel_op_sel      <= de_barrel_op_sel;
                end
                
                alu_op_sel         <= de_alu_op_sel;
                imm_output_en      <= de_imm_output_en;
                reg_read_B_en      <= de_reg_read_B_en;
                reg_write_en       <= de_reg_write_en;
                reg_lr_write_en    <= de_reg_lr_write_en;
                reg_cpsr_write_en  <= de_reg_cpsr_write_en;
            end
            
        else if (pipeline_current_state == PIPELINE_EXECUTE_STORE)
            begin
                data_out_reg_write_en <= DISABLE;
                
                mem_write_en <= ex_mem_write_en; // should always be ENABLED when STR and DISABLED when LDR
                data_out_sel <= ex_data_out_sel;
               
                if (ex_mem_write_en == 0) // OP LDR, don't update the address register yet
                begin
                    address_reg_sel <= ADDRESS_SELECT_PC;
                    update_address  <= DISABLE;
                    reg_pc_write_en <= DISABLE;
                end
                else
                begin
                    address_reg_sel <= ADDRESS_SELECT_PC;
                    update_address  <= ENABLE;
                    reg_pc_write_en <= DISABLE;
                end
               
                // post-indexed second cycle Rn update
                if (data_out_reg_write_en == ENABLE && ex_data_out_sel == DATA_OUT_LATCHED)
                begin
                    reg_write_en <= ENABLE;
                    
                    // b bus tri-state, set immediate offset on B-bus
                    reg_read_B_en <= DISABLE;
                    imm_output_en <= ENABLE;
                    data_prov_b_bus_en <= DISABLE;
                    
                    barrel_shift_val   <= de_barrel_shift_val;
                    barrel_op_sel      <= de_barrel_op_sel;
                end
                else // pre-indexed, immediate offset
                begin
                    reg_write_en <= DISABLE; // in case of updating the base register with the calculated address (W == 1, pre-indexed)
                    
                    // b bus tri-state, set Rd on mem bus
                    reg_read_B_en <= ENABLE;
                    imm_output_en <= DISABLE;
                    data_prov_b_bus_en <= DISABLE;
                end
            end
            
        else if (pipeline_current_state == PIPELINE_EXECUTE_LOAD)
            begin
                update_address <= ENABLE; // update address register from PC
                reg_write_en   <= ENABLE; // write mem data to Rd
                
                reg_read_B_en      <= DISABLE;
                imm_output_en      <= DISABLE;
                data_prov_b_bus_en <= ENABLE;
                
                reg_write_sel      <= reg_read_B_sel; // pre/post-indexed addressing was writing Rn on second cycle
                                                      // now we get Rd and set it as the register to write

                // barrel shifter NOP   
                barrel_shift_val   <= 32'b0;
                barrel_op_sel      <= BARREL_OP_LSL;
                
                alu_op_sel <= ALU_OP_MOV;
            end
            
        else if (pipeline_current_state == PIPELINE_EXECUTE)
            begin
                stretch_mclk <= update_address; // at any state when we updated the address in the execute stage, we have to stretch if we used BRAM
                                                // e.g., possible stretch when we wrote to the PC in the previous stage, or we do B, BL op
            
                address_reg_sel <= ADDRESS_SELECT_INC;
                update_address  <= ENABLE;
                reg_pc_write_en <= ENABLE;
                
                // simply skip this block when pipelining
                reg_write_en          <= DISABLE; // don't do this when pipelining
                reg_cpsr_write_en     <= DISABLE; // don't do this when pipelining
                mem_write_en          <= DISABLE;
                data_out_sel          <= DATA_OUT_HIGH_IMP;
                data_out_reg_write_en <= DISABLE;
                reg_lr_write_en       <= DISABLE;
                // ------------------------------------------------------------------------
            end
    end
    
endmodule
