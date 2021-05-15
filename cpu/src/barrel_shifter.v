`timescale 1ns / 1ps

/* Data-processing addressing modes, ARM ARM page A5-2 (pdf: p442) */
// notes:
//  - Rs should be read in the decode phase
//
//  register shift:
//      - Rm is shifted by the value in the least significant byte (8bits) of register Rs
//  immediate shift:
//      - Rm is shifted by the value of a 5bit immediate.
    
module barrel_shifter(
    input  wire [31:0] in_data,
           wire [7:0]  shift_value, // if immediate it is only 5 bits, if register all bits are used
           wire [31:0] in_op_select,
           wire        in_carry,
    output reg  [31:0] out_shifted_data,
           reg         out_carry
    );
    
    localparam op_LSL = 3'b000, // logical shift left
               op_LSR = 3'b001, // logical shift right
               op_ASR = 3'b010, // arithmetic shift right
               op_ROR = 3'b011, // rotate right
               op_RRX = 3'b100; // rotate right with extend
    
    reg [63:0] rotated_container;
    
    /* Carry */
    always @ (in_op_select, in_data, shift_value)
    begin
        if (shift_value == 0) // for carry this is the case at every operation
            out_carry = in_carry;
        else begin
            case (in_op_select)
                op_LSL : begin
                            if (shift_value < 32)
                                out_carry = in_data[6'd32 - shift_value];
                            else if (shift_value == 32)
                                out_carry = in_data[0];
                             else
                                out_carry = 0;
                         end
                op_LSR : begin
                            if (shift_value < 32)
                                out_carry = in_data[shift_value - 1];
                            else if (shift_value == 32)
                                out_carry = in_data[31];
                            else
                                out_carry = 0;
                         end
                op_ASR : begin
                            if (shift_value < 32)
                                out_carry = in_data[shift_value - 1];
                            else
                                out_carry = in_data[31];
                         end
                op_ROR : begin
                            if (shift_value[4:0] == 0)
                                out_carry = in_data[31];
                            else
                                out_carry = in_data[shift_value[4:0] - 1];
                         end
                op_RRX : out_carry = in_data[0];
            endcase
        end 
    end
    
    /* Shifted output */
    always @ (in_op_select, in_data, shift_value)
    begin
        if (shift_value == 0) // the operation doesn't matter in this case
            out_shifted_data = in_data;
        else begin
            case (in_op_select)
                op_LSL : begin
                            if (shift_value < 32) 
                                out_shifted_data = in_data << shift_value;
                            else
                                out_shifted_data = 0;
                         end
                op_LSR : begin
                            if (shift_value < 32)
                                out_shifted_data = in_data >> shift_value;
                            else 
                                out_shifted_data = 0;
                         end
                op_ASR : begin
                            if (shift_value < 32)
                                out_shifted_data = in_data >>> shift_value;
                            else
                                if (in_data == 0)
                                    out_shifted_data = 0;
                                else
                                    out_shifted_data = 32'hFFFF_FFFF;
                         end
                op_ROR : begin
                            if (shift_value[4:0] == 0)
                                out_shifted_data = in_data;
                            else begin
                                rotated_container = {in_data, 32'b0} >> shift_value[4:0];
                                out_shifted_data  = rotated_container[63:32] | rotated_container[31:0];
                            end
                         end
                op_RRX : begin // decode phase should convert the shift_value for RRX to anything other than 8'b0, because 0 is handled differently by this module
                            out_shifted_data = {in_carry, in_data[31:1]}; // logical shift right with carry replacing vacated bit position
                         end
            endcase
        end
    end
    
endmodule