`timescale 1ns / 1ps

module mmio_mapper(
    // UART INTERFACE
    input  wire  [7:0] in_uart_data,
           wire  [2:0] in_uart_status, // { rx_data_valid, tx_active, tx_done }
    output reg         out_uart_send_en,
           reg  [31:0] out_uart_data,
           reg         out_uart_data_is_read,
    
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
            // UART TX transmit data (addr: 0x0038_0000)
            if (in_address == 12'd0)
            begin
                out_uart_data         = in_write_data;
                out_uart_send_en      = in_write_en;
                out_uart_data_is_read = 0;
                out_read_data         = 32'b0;
            end
            
            // UART RX receive data (addr: 0x0038_0001)
            else if (in_address == 12'd1)
            begin
                out_uart_data         = 32'b0;
                out_uart_send_en      = 1'b0;
                out_uart_data_is_read = 0;
                out_read_data         = { 24'b0, in_uart_data };
            end
            
            // UART get 3bit status --> { rx_data_valid, tx_active, tx_done } (addr: 0x0038_0002)
            else if (in_address == 12'd2)
            begin
                out_uart_data         = 32'b0;
                out_uart_send_en      = 1'b0;
                out_uart_data_is_read = 0;
                out_read_data         = { 29'b0, in_uart_status };
            end
            
            // UART signal that data has been read --> STR Rd, [0x0038_0003]
            else if (in_address == 12'd3)
            begin
                out_uart_data         = 32'b0;
                out_uart_send_en      = 1'b0;
                out_uart_data_is_read = in_write_en;
                out_read_data         = 32'b0;
            end
            
            // reserved for later use - unimplemented
            else
            begin
                out_uart_data         = 32'b0;
                out_uart_send_en      = DISABLE;
                out_uart_data_is_read = 0;
                out_read_data         = 32'b0;
            end
        end
        
        // reserved for later use - unimplemented
        else
        begin
            out_uart_data         = 32'b0;
            out_uart_send_en      = DISABLE;
            out_uart_data_is_read = 0;
            out_read_data         = 32'b0;
        end
    end
    
endmodule
