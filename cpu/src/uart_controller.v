`timescale 1ns / 1ps

module uart_controller(
    input  wire        clk,
    
    // Basys board interface
    input  wire        in_serial,
    output wire        out_serial,

    // MMIO Mapper interface
    input  wire        in_send_data_en,
           wire [31:0] in_data,
           wire        in_send_word,    // whether to send just one byte, or a word
           wire        in_data_is_read, // signals the UART that the new data has been read, this will set the rx_data_valid to 0
    output wire  [7:0] out_data,
           wire  [2:0] out_uart_status // { rx_data_valid, tx_active, tx_done }
    );

    reg  uart_rx_new_data = 0;
    wire rx_data_valid;
    // reg  rx_send_data = 1'b0; // for words
    wire tx_is_active;
    wire tx_done;

    assign out_uart_status = { uart_rx_new_data, tx_is_active, tx_done };

    uart_tx  #(.BaudRate(115200), .ClockSpeed_MHz(100)) 
        uart_tx_inst (.clk(clk),
                      .in_send_data_en(in_send_data_en),
                      .in_data(in_data[7:0]),
                      .out_is_active(tx_is_active),
                      .out_serial(out_serial),
                      .out_done(tx_done));

    uart_rx  //#(.BaudRate(115200), .ClockSpeed_MHz(100)) 
        uart_rx_inst (.clk(clk),
                      .in_serial(in_serial),
                      .out_data(out_data),
                      .out_data_valid(rx_data_valid));

    /* If new data is received, signal new data for 100 clocks */
    always @ (posedge clk)
    begin
        if (in_data_is_read)
        begin
            uart_rx_new_data <= 0;
        end
        else if (rx_data_valid)
        begin
            uart_rx_new_data <= 1;
        end
    end

/* Later if we want to transmit whole words instead of bytes*/
/*
    localparam UART_TX_IDLE      = 2'b00,
               UART_TX_SEND_WORD = 2'b01;

    reg [31:0] word_to_send    = 32'b0;
    reg [1:0]  send_word_state = UART_TX_IDLE;
    reg [1:0]  bytes_sent      = 2'b0;
    
    always @ (posedge clk)
    begin
        if (send_word_state == UART_TX_IDLE)
        begin
            if (in_send_data_en)
            begin
                rx_send_data    <= 1;
                word_to_send    <= in_data;
                send_word_state <= UART_TX_SEND_WORD;
            end
        end
        
        else if (send_word_state == UART_TX_SEND_WORD)
        begin
            word_to_send <= word_to_send >> 8;
        
            if (bytes_sent < 3)
                bytes_sent <= bytes_sent + 1;
            else
                send_word_state <= UART_TX_IDLE;
        end

    end
*/
    
endmodule
