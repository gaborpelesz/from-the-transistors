`timescale 1ns / 1ps

/* instruction decoder and logic control */
module logic_control(
    input  wire        clk,
           wire        reset,
           wire [31:0] mem_data_prov_instruction,
    output reg         mem_read_en,
           reg         mem_write_en,
           reg   [3:0] reg_read_A_sel,
           reg   [3:0] reg_read_B_sel,
           reg         reg_read_B_en,
           reg   [3:0] reg_write_sel,
           reg         reg_write_en,
           reg         reg_pc_write_en,
           reg         reg_cpsr_write_en,
           reg   [1:0] address_reg_sel,
           reg   [7:0] barrel_shift_val,
           reg   [2:0] barrel_op_sel,
           reg   [3:0] alu_op_sel,
           reg         data_prov_out_sel,
           reg         data_out_en,
           reg         control_reset
    );
    
    localparam ADDRESS_SELECT_ALU = 2'b00,
               ADDRESS_SELECT_PC  = 2'b01,
               ADDRESS_SELECT_INC = 2'b10;
    
    // fetch-decode pipeline registers
    reg [31:0] fd_instruction;
    
    // decode-execute pipeline registers
    reg [3:0] de_reg_src_A;
    reg [3:0] de_reg_src_B;
    reg [3:0] de_reg_dest;
    reg       de_reg_write_en;
    reg       de_reg_write_pc_en;
    reg       de_reg_cpsr_write_en;
    reg [1:0] de_addreg_sel;
    reg       de_addreg_update;
    reg [7:0] de_barrel_shift_val;
    reg [2:0] de_barrel_op_sel;
    reg [3:0] de_alu_op_sel;
    
    localparam R0  = 4'd00, R1  = 4'd01, R2  = 4'd02, R3  = 4'd03,
               R4  = 4'd04, R5  = 4'd05, R6  = 4'd06, R7  = 4'd07,
               R8  = 4'd08, R9  = 4'd09, R10 = 4'd10, R11 = 4'd11,
               R12 = 4'd12, R13 = 4'd13, R14 = 4'd14, R15 = 4'd15;
               
    localparam BARREL_OP_LSL = 3'b000, // logical shift left
               BARREL_OP_LSR = 3'b001, // logical shift right
               BARREL_OP_ASR = 3'b010, // arithmetic shift right
               BARREL_OP_ROR = 3'b011, // rotate right
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
    
    localparam PIPELINE_RESET_1 = 3'b00,
               PIPELINE_RESET_2 = 3'b01,
               PIPELINE_FETCH   = 3'b10;
    reg [2:0] pipeline_state_reg = PIPELINE_RESET_1;
    
    always @ (posedge clk)
    begin
        // Reset
        if (pipeline_state_reg == PIPELINE_RESET_1)
        begin
            control_reset      <= ENABLE;
        
            address_reg_sel    <= ADDRESS_SELECT_PC;
            data_prov_out_sel  <= DATA_PROV_INSTRUCTION;
            data_out_en        <= DISABLE;
            
            reg_write_en       <= DISABLE;
            reg_pc_write_en    <= DISABLE;
            reg_cpsr_write_en  <= DISABLE;
            reg_read_B_en      <= ENABLE;
            reg_read_A_sel     <= R0;
            reg_read_B_sel     <= R0;
            reg_write_sel      <= R0;
            
            mem_read_en        <= DISABLE;
            mem_write_en       <= DISABLE;
            
            // barrel shifter NOP
            barrel_shift_val   <= 8'b0;
            barrel_op_sel      <= BARREL_OP_LSL;
            
            alu_op_sel         <= ALU_OP_MOV;
            
            pipeline_state_reg <= PIPELINE_RESET_2;
        end 
        else if (pipeline_state_reg == PIPELINE_RESET_2)
        begin
            control_reset      <= DISABLE;
            pipeline_state_reg <= PIPELINE_FETCH;
        end
        
        else if (pipeline_state_reg == PIPELINE_FETCH)
        begin
            mem_read_en       <= ENABLE;
            data_prov_out_sel <= DATA_PROV_INSTRUCTION;
        end
    end
    
    always @ (negedge clk)
    begin
        if (pipeline_state_reg == PIPELINE_FETCH)
        begin
            fd_instruction <= mem_data_prov_instruction;
            pipeline_state_reg <= PIPELINE_RESET_1;
        end
    end
    
endmodule
