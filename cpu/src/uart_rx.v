`timescale 1ns / 1ps

module uart_rx #(
    parameter BaudRate = 115200,
    parameter ClockSpeed_MHz = 100 // in MHz
) (
    input wire       clk,
          wire       in_serial,
    output reg [7:0] out_data,
           reg       out_data_valid
    );
    
    localparam CLOCK_SPEED = ClockSpeed_MHz * 1_000_000, // 100 MHz
               BAUD_COUNTER_MAX = CLOCK_SPEED / BaudRate;
    
    localparam STATE_INIT          = 3'd0,
               STATE_WAIT_FOR_DATA = 3'd1, 
               STATE_START_RECEIVE = 3'd2,
               STATE_RECEIVE_DATA  = 3'd3,
               STATE_STOP_BIT      = 3'd4,
               STATE_FINISH        = 3'd5;
    
    reg [2:0] rx_state = STATE_INIT;
    reg [9:0] baud_counter = 10'd0; // would be 19 bits for: baud rate = 300
    reg [4:0] received_bits_counter = 5'd0;
    reg [7:0] received_data_byte = 8'd0;
    
    always @ (posedge clk)
    begin
        if (rx_state == STATE_INIT)
        begin
            out_data <= 8'd0;
            out_data_valid <= 0;
            rx_state <= STATE_WAIT_FOR_DATA;
        end
    
        /* IDLE and wait on START BIT */
        else if (rx_state == STATE_WAIT_FOR_DATA)
        begin
            if (in_serial == 0)
            begin
                //out_data_valid <= 0;
                //out_data       <= 0;
            
                received_bits_counter <= 5'd0;
                received_data_byte = 8'd0;

                baud_counter <= 10'd1; // already counting 1 for this state

                rx_state <= STATE_START_RECEIVE;
            end
            else
            begin
                rx_state <= STATE_WAIT_FOR_DATA;
            end
        end
        
        /* START BIT */
        else if (rx_state == STATE_START_RECEIVE)
        begin
            // count half of the baud rate and check if it is 0 -> start bit
            // if not 0 -> go back to STATE_WAIT_FOR_DATA
            
            if (baud_counter < (BAUD_COUNTER_MAX / 2) - 1)
            begin
                baud_counter <= baud_counter + 1; 
            end
            
            else
            begin
                baud_counter <= 10'd0;
                
                if (in_serial == 0)
                    rx_state <= STATE_RECEIVE_DATA;
                else
                    rx_state <= STATE_WAIT_FOR_DATA;
            end
        end
        
        /* RECEIVING DATA */
        else if (rx_state == STATE_RECEIVE_DATA)
        begin
            // count one baud cycle
            if (baud_counter < (BAUD_COUNTER_MAX-1))
            begin
                baud_counter <= baud_counter + 1; 
            end
            
            else
            begin
                baud_counter       <= 10'd0;
                received_data_byte <= {in_serial, received_data_byte[7:1]};
                
                if (received_bits_counter < 5'd7)
                begin
                    received_bits_counter <= received_bits_counter + 1;
                end
                else
                begin
                    rx_state <= STATE_STOP_BIT;
                end
            end
        
        end
        
        else if (rx_state == STATE_STOP_BIT)
        begin
        
            if (baud_counter < (BAUD_COUNTER_MAX-1))
            begin
                baud_counter <= baud_counter + 1; 
            end
            
            else
            begin
                baud_counter <= 10'd0;
                rx_state <= STATE_FINISH;
                
                if (in_serial) // is stop bit
                begin
                    out_data <= received_data_byte;
                    out_data_valid <= 1;
                end
                
                else
                begin
                    out_data <= 8'd0;
                    out_data_valid <= 0;
                end
            end
        end
        
        else if (rx_state == STATE_FINISH)
        begin
            if (baud_counter < (BAUD_COUNTER_MAX / 2) - 1)
                baud_counter <= baud_counter + 1;
            else
                rx_state <= STATE_WAIT_FOR_DATA;
        end
        
        else
        begin
            out_data <= 8'd0;
            out_data_valid <= 0;
            rx_state <= STATE_WAIT_FOR_DATA;
        end
    end
        
endmodule