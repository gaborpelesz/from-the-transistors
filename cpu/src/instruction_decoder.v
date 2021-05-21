`timescale 1ns / 1ps

module instruction_decoder(
    input  wire [31:0] fd_instruction,
           wire [31:0] reg_shifter_value,
    output reg   [3:0] de_reg_read_A_sel,
           reg   [3:0] de_reg_read_B_sel,
           reg   [3:0] reg_read_C_sel,
           reg   [3:0] de_reg_write_sel,
           reg   [2:0] de_barrel_op_sel,
           reg   [3:0] de_alu_op_sel,
           reg  [31:0] de_barrel_shift_val,           
           reg  [31:0] de_immediate_value,
           reg         de_reg_read_B_en,
           reg         de_data_prov_b_bus_en,
           reg         de_imm_output_en,
           reg         de_reg_write_en,
           reg         de_reg_pc_write_en,
           reg         de_reg_cpsr_write_en,
           reg         de_data_out_en,
           reg         de_mem_write_en,
           reg         de_addreg_update,
           reg   [1:0] de_addreg_sel
    );
    
    localparam R0  = 4'd00, R1  = 4'd01, R2  = 4'd02, R3  = 4'd03,
           R4  = 4'd04, R5  = 4'd05, R6  = 4'd06, R7  = 4'd07,
           R8  = 4'd08, R9  = 4'd09, R10 = 4'd10, R11 = 4'd11,
           R12 = 4'd12, R13 = 4'd13, R14 = 4'd14, R15 = 4'd15;

    localparam DECODE_DATA_PROC_IMM_SHIFT  = 4'b0000,
               DECODE_DATA_PROC_REG_SHIFT  = 4'b0001,
               DECODE_DATA_PROC_IMM_1      = 4'b0010,
               DECODE_DATA_PROC_IMM_2      = 4'b0011,
               DECODE_LOAD_STORE_IMM_OFF_1 = 4'b0100,
               DECODE_LOAD_STORE_IMM_OFF_2 = 4'b0101,
               DECODE_LOAD_STORE_REG_OFF   = 4'b0110,
               DECODE_BRANCH_AND_BL_1      = 4'b1010,
               DECODE_BRANCH_AND_BL_2      = 4'b1011;
               
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
               
    localparam ADDRESS_SELECT_ALU = 2'b00,
               ADDRESS_SELECT_PC  = 2'b01,
               ADDRESS_SELECT_INC = 2'b10;

    reg [3:0] decoded_instruction_type;
    
    always @ (*)
    begin
        decoded_instruction_type = {fd_instruction[27:25], fd_instruction[4]}; // ARM ARM p110
    
        if (decoded_instruction_type == DECODE_DATA_PROC_IMM_SHIFT)
            begin
                // B bus tri-state
                de_reg_read_B_en      = ENABLE;
                de_data_prov_b_bus_en = DISABLE;
                de_imm_output_en      = DISABLE;
                
                de_reg_write_en       = ENABLE;            
                de_reg_pc_write_en    = DISABLE;
                de_reg_cpsr_write_en  = fd_instruction[20];
                
                de_data_out_en        = DISABLE;
                de_mem_write_en       = DISABLE;
                
                de_addreg_update      = DISABLE;
                de_addreg_sel         = ADDRESS_SELECT_INC; // change this if branching or jump
            
                de_reg_read_A_sel = fd_instruction[19:16]; // Rn
                de_reg_read_B_sel = fd_instruction[3:0];   // Rm
                reg_read_C_sel = R0;                       // Rs
                de_reg_write_sel  = fd_instruction[15:12]; // Rd
                
                de_barrel_op_sel  = fd_instruction[11:7] == 0 ? BARREL_OP_RRX : {1'b0, fd_instruction[6:5]};
                de_alu_op_sel     = fd_instruction[24:21];
                
                de_barrel_shift_val = {27'b0, fd_instruction[11:7]};
                de_immediate_value  = 32'b0;
            end
        else if (decoded_instruction_type == DECODE_DATA_PROC_REG_SHIFT)
            begin
                // B bus tri-state
                de_reg_read_B_en      = ENABLE;
                de_data_prov_b_bus_en = DISABLE;
                de_imm_output_en      = DISABLE;
                
                de_reg_write_en       = ENABLE;            
                de_reg_pc_write_en    = DISABLE;
                de_reg_cpsr_write_en  = fd_instruction[20];
                
                de_data_out_en        = DISABLE;
                de_mem_write_en       = DISABLE;
                
                de_addreg_update      = DISABLE;
                de_addreg_sel         = ADDRESS_SELECT_INC; // change this if branching or jump

                de_reg_read_A_sel = fd_instruction[19:16]; // Rn
                de_reg_read_B_sel = fd_instruction[3:0];   // Rm
                reg_read_C_sel    = fd_instruction[11:8];  // Rs
                de_reg_write_sel  = fd_instruction[15:12]; // Rd
                
                de_barrel_op_sel  = {1'b0, fd_instruction[6:5]}; // no RRX with register shift
                de_alu_op_sel     = fd_instruction[24:21];
                
                de_barrel_shift_val = reg_shifter_value;
                de_immediate_value  = 32'b0;
            end
            
        else if (decoded_instruction_type == DECODE_DATA_PROC_IMM_1 || decoded_instruction_type == DECODE_DATA_PROC_IMM_2)
            begin
                // B bus tri-state
                de_reg_read_B_en      = DISABLE;
                de_data_prov_b_bus_en = DISABLE;
                de_imm_output_en      = ENABLE;
                
                de_reg_write_en       = ENABLE;            
                de_reg_pc_write_en    = DISABLE;
                de_reg_cpsr_write_en  = fd_instruction[20];
                
                de_data_out_en        = DISABLE;
                de_mem_write_en       = DISABLE;
                
                de_addreg_update      = DISABLE;
                de_addreg_sel         = ADDRESS_SELECT_INC; // change this if branching or jump    
            
                de_reg_read_A_sel = fd_instruction[19:16]; // Rn
                de_reg_read_B_sel = R0;                    // Rm
                reg_read_C_sel    = R0;                    // Rs
                de_reg_write_sel  = fd_instruction[15:12]; // Rd
                
                de_barrel_op_sel  = {1'b0, fd_instruction[6:5]}; // no RRX with register shift
                de_alu_op_sel     = fd_instruction[24:21];
                
                de_barrel_shift_val = {27'b0, fd_instruction[11:8], 1'b0}; // shift_val = rotate_imm * 2
                de_immediate_value  = {24'b0, fd_instruction[7:0]};      
            end
    end
    
    
endmodule

