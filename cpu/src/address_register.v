`timescale 1ns / 1ps

module address_register(
    input  wire [31:0] in0, in1, in2,
           wire [1:0]  in_select,
    output wire [31:0] out_mem_address,
           wire [31:0] out_inc_address
    );
    
    reg [31:0] current_address = 32'b0;
    
    always @ (in_select, in0, in1, in2)
    begin
        case (in_select)
            2'b00   : current_address = in0; // ALU bus
            2'b01   : current_address = in1; // PC bus
            2'b10   : current_address = in2; // incrementer bus
            default : current_address = in2;
        endcase 
    end

    assign out_mem_address = current_address;
    assign out_inc_address = current_address;
    
endmodule
