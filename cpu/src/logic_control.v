`timescale 1ns / 1ps

/* instruction decoder and logic control */
module logic_control(
    input  wire        clk,
           wire        reset,
           wire [31:0] b_bus_in,
           wire [31:0] mem_data_prov_instruction,
    output reg         mem_write_en,
           reg   [3:0] reg_read_A_sel,
           reg   [3:0] reg_read_B_sel,
           reg         reg_read_B_en,
           reg   [3:0] reg_write_sel,
           reg         reg_write_en,
           reg         reg_pc_write_en,
           reg         reg_cpsr_write_en,
           reg   [1:0] address_reg_sel,
           reg         update_address,
           reg  [31:0] barrel_shift_val,
           reg   [2:0] barrel_op_sel,
           reg   [3:0] alu_op_sel,
           reg         data_prov_b_bus_en,
           reg         data_out_en,
           reg         control_reset,
           wire [31:0] out_immediate_value
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
               R12 = 4'd12, R13 = 4'd13, R14 = 4'd14, R15 = 4'd15;
               
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
    
    localparam PIPELINE_RESET_1 = 3'b000,
               PIPELINE_RESET_2 = 3'b001,
               PIPELINE_FETCH   = 3'b010,
               PIPELINE_DECODE  = 3'b011,
               PIPELINE_EXECUTE = 3'b100,
               PIPELINE_STALL   = 3'b101;
    
    reg [2:0] pipeline_current_state = PIPELINE_STALL;
    reg [2:0] pipeline_next_state    = PIPELINE_RESET_1;
    
    // fetch-decode pipeline registers
    reg [31:0] fd_instruction = 32'b11100001101000000000000000000000; // initialize to NOP (MOV R0, R0)
    
    wire [3:0] decoded_instruction_type; 
    assign decoded_instruction_type = {fd_instruction[27:25], fd_instruction[4]}; // ARM ARM p110   
    
    // decode-execute pipeline registers
    reg        de_mem_write_en       = DISABLE;
    reg  [3:0] de_reg_read_A_sel     = R0;
    reg  [3:0] de_reg_read_B_sel     = R0;
    reg        de_reg_read_B_en      = ENABLE;
    reg  [3:0] de_reg_write_sel      = R0;
    reg        de_reg_write_en       = DISABLE;
    reg        de_reg_pc_write_en    = DISABLE;
    reg        de_reg_cpsr_write_en  = DISABLE;
    reg  [1:0] de_addreg_sel         = ADDRESS_SELECT_PC;
    reg        de_addreg_update      = DISABLE;
    reg [31:0] de_barrel_shift_val   = 32'b0;
    reg  [2:0] de_barrel_op_sel      = BARREL_OP_LSL;
    reg  [3:0] de_alu_op_sel         = ALU_OP_MOV;
    reg        de_data_prov_b_bus_en = DISABLE;
    reg        de_data_out_en        = DISABLE;
    reg [31:0] de_immediate_value    = 32'hFFFF_FFFF;
    reg        de_imm_output_en      = DISABLE;

    /* Immediate data provider module init */
    reg         imm_output_en   = DISABLE;
    reg  [31:0] immediate_value = 32'hFFFF_FFFF;
    imm_data_provider imm_data_provider_inst (.in_immediate(immediate_value),
                                              .in_output_en(imm_output_en),
                                              .out_b_bus(out_immediate_value));
    
    reg [5:0] counter_stop_instead_of_jmp = 6'b0;
    
    always @ (posedge clk)
    begin
        if (counter_stop_instead_of_jmp == 30)
            pipeline_next_state <= PIPELINE_STALL;
        else
        begin
        
        counter_stop_instead_of_jmp <= counter_stop_instead_of_jmp + 1; 
        
        if (reset)
            pipeline_next_state <= PIPELINE_RESET_1;
    
        else if (pipeline_next_state == PIPELINE_RESET_1)
        begin
            update_address <= DISABLE;
        
            // reset registers in register bank
            control_reset      <= ENABLE;

            // select Address input
            address_reg_sel    <= ADDRESS_SELECT_INC;
            
            // select Rn, Rm, Rd 
            reg_read_A_sel     <= R0;
            reg_read_B_sel     <= R0;
            reg_write_sel      <= R0;
            
            // barrel shifter NOP
            barrel_shift_val   <= 32'b0;
            barrel_op_sel      <= BARREL_OP_LSL;
            
            // alu set operation to MOV
            alu_op_sel         <= ALU_OP_MOV;
            
            // disable write signals
            reg_write_en       <= DISABLE;
            reg_pc_write_en    <= DISABLE;
            reg_cpsr_write_en  <= DISABLE;
            
            // disable memory
            data_out_en        <= DISABLE;
            mem_write_en       <= DISABLE;
            
            pipeline_next_state <= PIPELINE_RESET_2;
        end 
        else if (pipeline_next_state == PIPELINE_RESET_2)
        begin
            control_reset       <= DISABLE;
            pipeline_next_state <= PIPELINE_FETCH;
        end
        
        else if (pipeline_next_state == PIPELINE_FETCH)
        begin
            fd_instruction     <= mem_data_prov_instruction;
            update_address     <= ENABLE;
            address_reg_sel    <= ADDRESS_SELECT_INC;
            reg_pc_write_en    <= ENABLE; // hardcoding no branch operation
            
            mem_write_en      <= DISABLE;
            data_out_en       <= DISABLE;
            
            reg_cpsr_write_en <= DISABLE;
            reg_write_en      <= DISABLE;
            
            pipeline_next_state <= PIPELINE_DECODE;
        end
        
        else if (pipeline_next_state == PIPELINE_DECODE)
        begin
            /* TODO:
                check if COND (fd_instruction[31:28]) is met with the current CPSR status signals.
                if not met, set instruction to execute a NOP (MOV R0, R0) operation
            */
        
            update_address  <= DISABLE; // fetch enables the update, here we need to disable it
            reg_pc_write_en <= DISABLE; // hardcoding no branch operation
        
            if (decoded_instruction_type == DECODE_DATA_PROC_IMM_SHIFT)
            begin
                // operands
                de_reg_read_A_sel <= fd_instruction[19:16];
                de_reg_read_B_sel <= fd_instruction[3:0];
                de_reg_write_sel  <= fd_instruction[15:12];
                // de_immediate_value; // not updated. In this case should we use de_immediate_value<=de_immediate_value; or de_immediate_value<=immediate_value?
                
                // Barrel shifter and ALU
                de_barrel_op_sel    <= fd_instruction[11:7] == 0 ? BARREL_OP_RRX : {1'b0, fd_instruction[6:5]};
                de_alu_op_sel       <= fd_instruction[24:21];
                
                // no update, is this okay?
                // de_addreg_sel <= ADDRESS_SELECT_PC; // is it right if we select PC instead of Incrementer bus? Will it delay?
                
                de_reg_write_en       <= ENABLE;
                de_data_out_en        <= DISABLE;
                de_reg_pc_write_en    <= DISABLE;
                de_mem_write_en       <= DISABLE;
                de_addreg_update      <= DISABLE;
                de_reg_cpsr_write_en  <= fd_instruction[20]; // check for S bit if set
                
                // B bus tri-state
                de_reg_read_B_en      <= ENABLE;
                de_data_prov_b_bus_en <= DISABLE;
                de_imm_output_en      <= DISABLE;
            end
            
            else if (decoded_instruction_type == DECODE_DATA_PROC_REG_SHIFT)
            begin
                // operands
                de_reg_read_A_sel <= fd_instruction[19:16];
                de_reg_read_B_sel <= fd_instruction[3:0];
                de_reg_write_sel  <= fd_instruction[15:12];
                // de_immediate_value; // not updated. In this case should we use de_immediate_value<=de_immediate_value; or de_immediate_value<=immediate_value?
                
                // reading the register on the B bus to save it as the shifter value
                reg_read_B_sel     <= fd_instruction[11:8];
                
                // Barrel shifter value will be read on negedge
                // de_barrel_shift_val
                de_barrel_op_sel    <= {1'b0, fd_instruction[6:5]}; // no RRX with register shift
                de_alu_op_sel       <= fd_instruction[24:21];
                
                // no update, is this okay?
                // de_addreg_sel <= ADDRESS_SELECT_PC; // is it right if we select PC instead of Incrementer bus? Will it delay?
                
                de_reg_write_en       <= ENABLE;
                de_data_out_en        <= DISABLE;
                de_reg_pc_write_en    <= DISABLE;
                de_mem_write_en       <= DISABLE;
                de_addreg_update      <= DISABLE;
                de_reg_cpsr_write_en  <= fd_instruction[20]; // check for S bit if set
                
                // B bus tri-state
                de_reg_read_B_en      <= ENABLE;
                de_data_prov_b_bus_en <= DISABLE;
                de_imm_output_en      <= DISABLE;
            end
            
            else if (decoded_instruction_type == DECODE_DATA_PROC_IMM_1 || decoded_instruction_type == DECODE_DATA_PROC_IMM_2)
            begin
                de_reg_read_A_sel  <= fd_instruction[19:16];
                // de_reg_read_B_sel <= R0; // no update? because it won't do anything
                de_reg_write_sel   <= fd_instruction[15:12];
                de_immediate_value <= {24'b0, fd_instruction[7:0]};
                
                // shifter operation is a ROR with a shifter value = rotate_imm * 2, where rotate_imm = instruction[11:8]
                de_barrel_op_sel    <= BARREL_OP_ROR;
                de_alu_op_sel       <= fd_instruction[24:21];
                
                de_reg_write_en       <= ENABLE;
                de_data_out_en        <= DISABLE;
                de_reg_pc_write_en    <= DISABLE;
                de_mem_write_en       <= DISABLE;
                de_addreg_update      <= DISABLE;
                de_reg_cpsr_write_en  <= fd_instruction[20]; // check for S bit if set
                
                // read Rn to A bus, and set the destination register. Register B bus is disabled.
                // B bus tri-state
                de_reg_read_B_en       <= DISABLE;
                de_imm_output_en       <= ENABLE;
                de_data_prov_b_bus_en  <= DISABLE;                
            end
            
            else if (decoded_instruction_type == DECODE_LOAD_STORE_IMM_OFF_1 || decoded_instruction_type == DECODE_LOAD_STORE_IMM_OFF_2)
            begin
                // In an LDR/STR immediate offset, the following bits are always: P_bit == 1, W_bit == 0 
                // U bit: addition or subtraction on the base register by the immediate offset
                de_alu_op_sel <= fd_instruction[23] == 0 ? ALU_OP_SUB : ALU_OP_ADD;
                // B bit: is unsigned
                // L bit: is load (or store)
            
                // de_reg_read_A_sel
            
                /*
                // operands
                
                de_reg_read_B_sel <= fd_instruction[3:0];
                de_reg_write_sel  <= fd_instruction[19:16];
                // de_immediate_value; // not updated. In this case should we use de_immediate_value<=de_immediate_value; or de_immediate_value<=immediate_value?
                
                // reading the register on the B bus to save it as the shifter value
                reg_read_B_sel     <= fd_instruction[11:8];
                reg_read_B_en      <= ENABLE;
                data_prov_b_bus_en <= DISABLE;
                imm_output_en      <= DISABLE;
                
                // Barrel shifter value will be read on negedge
                // de_barrel_shift_val
                de_barrel_op_sel    <= {1'b0, fd_instruction[6:5]}; // no RRX with register shift
                de_alu_op_sel       <= fd_instruction[24:21];
                
                // no update, is this okay?
                // de_addreg_sel <= ADDRESS_SELECT_PC; // is it right if we select PC instead of Incrementer bus? Will it delay?
                
                de_reg_write_en       <= ENABLE;
                de_data_out_en        <= DISABLE;
                de_reg_write_pc_en    <= DISABLE;
                de_mem_write_en       <= DISABLE;
                de_addreg_update      <= DISABLE;
                de_reg_cpsr_write_en  <= fd_instruction[20]; // check for S bit if set
                
                // B bus tri-state
                de_reg_read_B_en      <= ENABLE;
                de_data_prov_b_bus_en <= DISABLE;
                de_imm_output_en      <= DISABLE;
                */
            end
            
            else if (decoded_instruction_type == DECODE_LOAD_STORE_REG_OFF)
            begin
            end
            
            else if (decoded_instruction_type == DECODE_BRANCH_AND_BL_1 || decoded_instruction_type == DECODE_BRANCH_AND_BL_2)
            begin
            end
            
            pipeline_next_state <= PIPELINE_EXECUTE;
        end
        
        else if (pipeline_next_state == PIPELINE_EXECUTE)
        begin
            mem_write_en    <= de_mem_write_en;
            data_out_en     <= de_data_out_en;
            reg_pc_write_en <= de_reg_pc_write_en;
            update_address  <= de_addreg_update;
        
            immediate_value <= de_immediate_value;
            
            // get ALU operation
            alu_op_sel <= de_alu_op_sel;
            
            // shifter operation is a ROR with a shifter value = rotate_imm * 2, where rotate_imm = instruction[11:8]
            barrel_op_sel    <= de_barrel_op_sel;
            barrel_shift_val <= de_barrel_shift_val;
            
            // read Rn to A bus, and set the destination register. Register B bus is disabled.
            reg_read_A_sel <= de_reg_read_A_sel;
            reg_read_B_sel <= de_reg_read_B_sel;
            reg_write_sel  <= de_reg_write_sel;
            reg_write_en   <= de_reg_write_en;
            
            // check for S bit if set
            reg_cpsr_write_en <= de_reg_cpsr_write_en;
            
            pipeline_next_state <= PIPELINE_FETCH;
        end
        
        else
        begin
            reg_write_en   <= DISABLE;
            pipeline_next_state <= PIPELINE_STALL;
        end
        
        end // end stop instead of jmp
    end
    
    always @ (negedge clk)
    begin
        if (pipeline_next_state == PIPELINE_RESET_1)
        begin
            // B bus tri-state
            reg_read_B_en       <= DISABLE;
            imm_output_en       <= ENABLE;
            data_prov_b_bus_en  <= DISABLE;
        end
    
        else if (pipeline_next_state == PIPELINE_EXECUTE)
        begin
            // Read the register shifter value from the B bus
            // then save it as the barrel shifter value
            if (decoded_instruction_type == DECODE_DATA_PROC_REG_SHIFT)
            begin
                reg_read_B_en      <= ENABLE;
                data_prov_b_bus_en <= DISABLE;
                imm_output_en      <= DISABLE;
            
                de_barrel_shift_val <= b_bus_in; // read the shifter value from the B bus
            end
            else if (decoded_instruction_type == DECODE_DATA_PROC_IMM_SHIFT)
            begin
                de_barrel_shift_val <= {27'b0, fd_instruction[11:7]};
            end
            else if (decoded_instruction_type == DECODE_DATA_PROC_IMM_1 || decoded_instruction_type == DECODE_DATA_PROC_IMM_2)
            begin
                de_barrel_shift_val <= {27'b0, fd_instruction[11:8], 1'b0}; // shift_val = rotate_imm * 2
            end
            
            // B bus tri-state
            reg_read_B_en      <= de_reg_read_B_en;
            imm_output_en      <= de_imm_output_en;
            data_prov_b_bus_en <= de_data_prov_b_bus_en;
            
            // memory address register input select
            //address_reg_sel    <= de_addreg_sel;
        end
    end
    
endmodule
