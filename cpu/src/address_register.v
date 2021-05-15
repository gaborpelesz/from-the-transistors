`timescale 1ns / 1ps

module address_register(
    input  wire        clk,
           wire [31:0] in0, in1, in2,
           wire [1:0]  in_select,
           wire        perform_address_update,
    output wire  [31:0] out_mem_address,
           wire  [31:0] out_inc_address
    );
    
    reg [31:0] current_address = 32'b0;
    
    always @ (posedge clk)
    begin
        if (perform_address_update == 1'b1)
        begin
            case (in_select)
                2'b00   : current_address <= in0;
                2'b01   : current_address <= in1;
                2'b10   : current_address <= in2;
                default : current_address <= in2;
            endcase
        end 
    end

    assign out_mem_address = current_address;
    assign out_inc_address = current_address;
    
endmodule
