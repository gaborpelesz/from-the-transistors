`timescale 1ns / 1ps

module uart_tx #(
    parameter BaudRate = 115200,
    parameter ClockSpeed_MHz = 100 // in MHz
) (
    input wire       clk, 
          wire       in_send_data_en,
          wire [7:0] in_data,
    output reg       out_is_active,
           reg       out_serial,
           reg       out_done
    );
    
    localparam CLOCK_SPEED      = ClockSpeed_MHz * 1_000_000, // 100 MHz
               BAUD_COUNTER_MAX = CLOCK_SPEED / BaudRate;
               
    localparam STATE_WAIT_FOR_DATA   = 3'd0, 
               STATE_START_TRANSMIT  = 3'd1,
               STATE_TRANSMIT_BYTES  = 3'd2,
               STATE_STOP_BIT        = 3'd3, 
               STATE_FINISH_TRANSMIT = 3'd4,
               STATE_INITIALIZE      = 3'd5;
    
    reg [2:0] tx_state = STATE_INITIALIZE;
    reg [7:0] bytes_to_send = 8'd0;
    reg [9:0] baud_counter = 10'd0; // would be 19 bits for: baud rate = 300
    reg [4:0] transmitted_bits_counter = 5'd0;
    
    always @ (posedge clk)
    begin
        /* WAIR FOR TRIGGER */
        if (tx_state == STATE_WAIT_FOR_DATA) // if triggered
        begin
            if (in_send_data_en)
            begin        
                // init
                out_serial    <= 1;
                out_is_active <= 1;
                out_done      <= 0;
                
                bytes_to_send            <= in_data;
                baud_counter             <= 10'd0;
                transmitted_bits_counter <= 5'd0;
                
                tx_state <= STATE_START_TRANSMIT;
            end
            else // IDLE
            begin
                out_serial    <= 1;
                out_is_active <= 0;
                out_done      <= 0;
                
                tx_state <= STATE_WAIT_FOR_DATA;
            end

        end
        
        /* START BIT */
        else if (tx_state == STATE_START_TRANSMIT)
        begin
            out_serial    <= 0;
            out_is_active <= 1;
            out_done      <= 0;
            
            if (baud_counter < BAUD_COUNTER_MAX-1)
            begin
                baud_counter <= baud_counter + 1; 
            end
            else
            begin
                baud_counter <= 8'd0;
                tx_state <= STATE_TRANSMIT_BYTES;
            end
        end
        
        /* DATA TRANSFER */
        else if (tx_state == STATE_TRANSMIT_BYTES)
        begin
            out_serial    <= bytes_to_send[0];
            out_is_active <= 1;
            out_done      <= 0;
        
            // baud rate counter
            if (baud_counter < BAUD_COUNTER_MAX-1)
            begin
                baud_counter <= baud_counter + 1; 
            end

            // counted
            else
            begin
                baud_counter <= 8'd0; // reset counter
                
                if (transmitted_bits_counter < 5'd7)
                begin
                    transmitted_bits_counter <= transmitted_bits_counter + 1;
                    bytes_to_send <= bytes_to_send >> 1;
                end
                else
                begin
                    transmitted_bits_counter <= 8'd0;
                    tx_state <= STATE_STOP_BIT;
                end
            end
            
        end
        
        /* STOP BIT STATE */
        else if (tx_state == STATE_STOP_BIT)
        begin
            out_serial    <= 1;
            out_is_active <= 1;
        
            if (baud_counter < BAUD_COUNTER_MAX-1)
            begin
                baud_counter <= baud_counter + 1; 
            end
            else
            begin
                baud_counter <= 8'd0;
                out_done <= 1;
                tx_state <= STATE_FINISH_TRANSMIT;
            end
        end
        
        /* FINISH TRANSFER */
        else if (tx_state == STATE_FINISH_TRANSMIT)
        begin
            out_serial    <= 1;
            out_is_active <= 1;
            out_done      <= 1;

            tx_state <= STATE_WAIT_FOR_DATA;
        end
        
        /* INITIALIZE; DEFAULT TRIGGER WAITING STATE */
        else if (tx_state == STATE_INITIALIZE)
        begin
            out_serial    <= 1;
            out_is_active <= 0;
            out_done      <= 0;
            
            tx_state <= STATE_WAIT_FOR_DATA;
        end
        
    end
    
endmodule
