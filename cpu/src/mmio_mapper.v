`timescale 1ns / 1ps

module mmio_mapper(
    // UART INTERFACE
    input  wire  [7:0] in_uart_data,
           wire  [2:0] in_uart_status, // { rx_data_valid, tx_active, tx_done }
    output reg         out_uart_send_en,
           reg  [31:0] out_uart_data,
    
    // MEMORY MAPPER INTERFACE
    input  wire        in_reset,
           wire [11:0] in_address,
           wire [31:0] in_write_data,
           wire        in_write_en,
    output reg  [31:0] out_read_data
    );
    
    localparam ENABLE = 1'b1, DISABLE = 1'b0;
    
    always @ (*)
    begin
        // UART
        if (in_address >= 0 && in_address < 12'h400)
        begin
            // UART transmit data 0x0000
            if (in_address == 0)
            begin
                out_uart_data    = in_write_data;
                out_uart_send_en = in_write_en;
                out_read_data    = 32'b0;
            end
            else
            begin
                out_uart_data    = 32'b0;
                out_uart_send_en = DISABLE;
                out_read_data    = 32'b0;
            end
            // uart_send_data = in_write_en
            // out_data = in_write_data
            // if in_address == 0 then send_word = 0
            // if in_address == 4 then send_word = 1
        end
        else
        begin
            out_uart_data    = 32'b0;
            out_uart_send_en = DISABLE;
            out_read_data    = 32'b0;
        end
    end
    
endmodule
