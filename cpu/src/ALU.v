`timescale 1ns / 1ps

module ALU32(
    input  wire [3:0]  in_op_select,
           wire [31:0] in_data0, // A bus
           wire [31:0] in_data1, // B bus
           wire        in_carry,  // can carry for exmpl. ADC, or carry generated by the barrel shifter
           wire        in_overflow,
    output reg  [31:0] out_data,
           wire        out_neg,
           wire        out_zero,
           reg         out_carry,
           reg         out_overflow
    );
    
    /*  Data-processing instructions
        ARM ARM page 115
    */
    localparam
    op_AND = 4'b0000, // bitwise AND
    op_EOR = 4'b0001, // bitwise XOR
    op_SUB = 4'b0010, // subtract
    op_RSB = 4'b0011, // reverse subtract; note: to negate val in <Rm> use " RSBS <Rd>, <Rm>, #0 "
    op_ADD = 4'b0100, // add
    op_ADC = 4'b0101, // add with carry
    op_SBC = 4'b0110, // subtract with carry
    op_RSC = 4'b0111, // reverse subtract with carry
    op_TST = 4'b1000, // test
    op_TEQ = 4'b1001, // test equivalence
    op_CMP = 4'b1010, // compare
    op_CMN = 4'b1011, // compare negated
    op_ORR = 4'b1100, // or
    op_MOV = 4'b1101, // move, only important when updating CPSR
    op_BIC = 4'b1110, // bit clear
    op_MVN = 4'b1111; // move not, only important when updating CPSR
    
    /* ADDER MODULE */
    reg         add32_in_carry = 0;
    wire [31:0] add32_out_data;
    wire        add32_out_carry;
    wire        add32_out_overflow;
    add32 add32_inst (in_data0, in_data1, add32_in_carry, add32_out_data, add32_out_carry, add32_out_overflow);
    
    /* SUBTRACT MODULE */
    reg  [31:0] sub32_in_data0;
    reg  [31:0] sub32_in_data1;
    reg         sub32_in_carry;
    wire [31:0] sub32_out_data;
    wire        sub32_out_carry;
    wire        sub32_out_overflow;
    sub32 sub32_inst (sub32_in_data0, sub32_in_data1, sub32_in_carry, sub32_out_data, sub32_out_carry, sub32_out_overflow);
    
    always @ (*)
    begin
        /* ALU operation set input */
        if (in_op_select == op_SBC || in_op_select == op_RSC)
            sub32_in_carry = in_carry;
        else
            sub32_in_carry = 1;
        
        if (in_op_select == op_RSB || in_op_select == op_RSC)
        begin
            sub32_in_data0 = in_data0;
            sub32_in_data1 = in_data1;
        end
        else
        begin
            sub32_in_data0 = in_data1;
            sub32_in_data1 = in_data0;
        end
            
        if (in_op_select == op_ADC)
            add32_in_carry = in_carry;
        else
            add32_in_carry = 0;
        
    
        /* ALU output data */
        case (in_op_select)
            op_AND,
                op_TST : out_data =  in_data0  &  in_data1;
            op_EOR,
                op_TEQ : out_data =  in_data0  ^  in_data1;
            op_ORR     : out_data =  in_data0  |  in_data1;
            op_BIC     : out_data =  in_data0  & ~in_data1;
            op_MOV     : out_data =  in_data1;
            op_MVN     : out_data = ~in_data1;
            op_SUB,
                op_SBC,
                op_RSB,
                op_RSC,
                op_CMP : out_data = sub32_out_data;
            op_ADD,
                op_CMN,
                op_ADC : out_data = add32_out_data;
        endcase
        
        /* ALU output flags */
        if (in_op_select == op_SUB || in_op_select == op_CMP || in_op_select == op_RSB || in_op_select == op_SBC || in_op_select == op_RSC)
        begin
            out_carry    = sub32_out_carry;
            out_overflow = sub32_out_overflow;
        end
        else if (in_op_select == op_ADD || in_op_select == op_CMN || in_op_select == op_ADC)
        begin
            out_carry    = add32_out_carry;
            out_overflow = add32_out_overflow;
        end
        else
        begin
            out_carry    = in_carry;
            out_overflow = in_overflow;
        end
    end // always @ (op_ALU)
    
    /* Wireable flags */
    assign out_neg  = out_data[31];
    assign out_zero = out_data == 0;
    
endmodule
