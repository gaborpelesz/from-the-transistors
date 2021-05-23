`timescale 1ns / 1ps

module conditional_evaluator(
    input  wire [3:0] in_cpsr,
           wire [3:0] in_cond,
    output reg        out_execute_en
    );
    
    localparam CONDITION_EQ     = 4'b0000, // Z set            => equal
               CONDITION_NE     = 4'b0001, // Z clear          => not equal 
               CONDITION_CS_HS  = 4'b0010, // C set            => carry set / unsigned higher or same 
               CONDITION_CC_LO  = 4'b0011, // C clear          => carry clear / unsigned lower
               CONDITION_MI     = 4'b0100, // N set            => minus / negative
               CONDITION_PL     = 4'b0101, // N clear          => plus / positive or zero
               CONDITION_VS     = 4'b0110, // V set            => overflow
               CONDITION_VC     = 4'b0111, // V clear          => no overflow
               CONDITION_HI     = 4'b1000, // C set & Z clear  => unsigned higher 
               CONDITION_LS     = 4'b1001, // C clear & Z set  => unsigned lower or same
               CONDITION_GE     = 4'b1010, // (N == V)         => signed greater than or equal to (N & V | ~N & ~V) 
               CONDITION_LT     = 4'b1011, // (N != V)         => signed less than (N & ~V | ~N & V)
               CONDITION_GT     = 4'b1100, // (Z == 0, N == V) => signed greater than
               CONDITION_LE     = 4'b1101, // (Z == 1, N != V) => signed less than or equal
               CONDITION_AL     = 4'b1110, // ...              => always (unconditional)
               CONDITION_UNPRED = 4'b1111; // ...              => unpredictable behavior
           
    wire N, Z, C, V;
    assign { N, Z, C, V } = in_cpsr[3:0];
    
    always @ (*)
    begin
        case (in_cond)
            CONDITION_EQ     : out_execute_en =  Z;
            CONDITION_NE     : out_execute_en = ~Z;
            CONDITION_CS_HS  : out_execute_en =  C;
            CONDITION_CC_LO  : out_execute_en = ~C;
            CONDITION_MI     : out_execute_en =  N;
            CONDITION_PL     : out_execute_en = ~N;
            CONDITION_VS     : out_execute_en =  V;
            CONDITION_VC     : out_execute_en = ~V;
            CONDITION_HI     : out_execute_en =  C & ~Z;
            CONDITION_LS     : out_execute_en = ~C &  Z;
            CONDITION_GE     : out_execute_en = N == V;
            CONDITION_LT     : out_execute_en = N != V;
            CONDITION_GT     : out_execute_en = ~Z & (N == V);
            CONDITION_LE     : out_execute_en =  Z & (N != V);
            CONDITION_AL     : out_execute_en = 1'b1;
            CONDITION_UNPRED : out_execute_en = 1'b0;
            default          : out_execute_en = 1'b0; // should never happen
        endcase
    end
    
endmodule
