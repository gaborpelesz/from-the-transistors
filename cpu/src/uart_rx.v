`timescale 1ns / 1ps

module uart_rx (
    input  wire       clk,
           wire       in_serial,
    output wire [7:0] out_data,
           reg        out_data_valid
    );
    
//internal variables
reg shift; // shift signal to trigger shifting data
reg state = 0, nextstate; // initial state and next state variable
reg [3:0] bitcounter = 0; // 4 bits counter to count up to 9 for UART receiving
reg [1:0] samplecounter = 0; // 2 bits sample counter to count up to 4 for oversampling
reg [13:0] counter = 0; // 14 bits counter to count the baud rate
reg [9:0] rxshiftreg = 0; //bit shifting register
reg clear_bitcounter,inc_bitcounter,inc_samplecounter,clear_samplecounter; //clear or increment the counter

reg set_out_data_valid;

// constants
localparam clk_freq = 100_000_000;  // system clock frequency
localparam baud_rate = 115_200; //baud rate
localparam div_sample = 4; //oversampling
localparam div_counter = clk_freq/(baud_rate*div_sample);  // this is the number we have to divide the system clock frequency to get a frequency (div_sample) time higher than (baud_rate)
localparam mid_sample = (div_sample/2);  // this is the middle point of a bit where you want to sample it
localparam div_bit = 10; // 1 start, 8 data, 1 stop


assign out_data = rxshiftreg [8:1]; // assign the RxData from the shiftregister

//UART receiver logic
always @ (posedge clk)
    begin 
        counter <= counter +1; // start count in the counter
        if (counter >= div_counter-1) // if counter reach the baud rate with sampling
        begin 
            counter <=0; //reset the counter
            state <= nextstate; // assign the state to nextstate
            
            if (shift)
                rxshiftreg <= {in_serial,rxshiftreg[9:1]}; //if shift asserted, load the receiving data
                
            if (clear_samplecounter)
                samplecounter <=0; // if clear sampl counter asserted, reset sample counter
                
            if (inc_samplecounter)
                samplecounter <= samplecounter +1; //if increment counter asserted, start sample count
                
            if (clear_bitcounter)
                bitcounter <=0; // if clear bit counter asserted, reset bit counter
                
            if (inc_bitcounter)
                bitcounter <= bitcounter +1; // if increment bit counter asserted, start count bit counter
                
            if (set_out_data_valid)
                out_data_valid <= 1;
            else
                out_data_valid <= 0;
        end
        else
        begin
            out_data_valid <= 0;
        end
    end
   
//state machine

always @ (posedge clk) //trigger by clock
begin
    set_out_data_valid <= 0;
    shift <= 0; // set shift to 0 to avoid any shifting 
    clear_samplecounter <=0; // set clear sample counter to 0 to avoid reset
    inc_samplecounter <=0; // set increment sample counter to 0 to avoid any increment
    clear_bitcounter <=0; // set clear bit counter to 0 to avoid claring
    inc_bitcounter <=0; // set increment bit counter to avoid any count
    nextstate <=0; // set next state to be idle state
    case (state)
        0: begin // idle state
            if (in_serial) // if input RxD data line asserted
              begin
              nextstate <=0; // back to idle state because RxD needs to be low to start transmission    
              end
            else begin // if input RxD data line is not asserted
                nextstate <=1; //jump to receiving state 
                clear_bitcounter <=1; // trigger to clear bit counter
                clear_samplecounter <=1; // trigger to clear sample counter
            end
        end
        1: begin // receiving state
            if (samplecounter== mid_sample - 1) 
                shift <= 1; // if sample counter is 1, trigger shift
                 
            if (samplecounter== div_sample - 1) // if sample counter is 3 as the sample rate used is 3
            begin 
                
                if (bitcounter == div_bit - 1) // check if bit counter if 9 or not
                begin
                    nextstate <= 0; // back to idle state if bit counter is 9 as receving is complete
                    set_out_data_valid <= 1;
                end
                else
                    nextstate <= 1;
                 
                inc_bitcounter <=1; // trigger the increment bit counter if bit counter is not 9
                clear_samplecounter <=1; //trigger the sample counter to reset the sample counter
            end
             
            else
            begin
                nextstate <= 1;
                inc_samplecounter <=1; // if sample is not equal to 3, keep counting
            end
        end
       default: nextstate <=0; //default idle state
     endcase
end         
endmodule

/*
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
    
    reg                        [2:0] rx_state              = STATE_INIT;
    reg [$clog2(BAUD_COUNTER_MAX):0] baud_counter          = 0;
    reg                        [4:0] received_bits_counter = 5'd0;
    reg                        [7:0] received_data_byte    = 8'd0;
    
    always @ (posedge clk)
    begin
        if (rx_state == STATE_INIT)
        begin
            out_data <= 8'd0;
            out_data_valid <= 0;
            rx_state <= STATE_WAIT_FOR_DATA;
        end
    
        // IDLE and wait on START BIT
        else if (rx_state == STATE_WAIT_FOR_DATA)
        begin
            out_data_valid <= 0;
            
            if (in_serial == 0)
            begin
                received_bits_counter <= 5'd0;
                received_data_byte = 8'd0;

                baud_counter <= 1; // already counting 1 for this state

                rx_state <= STATE_START_RECEIVE;
            end
            else
            begin
                rx_state <= STATE_WAIT_FOR_DATA;
            end
        end
        
        // START BIT 
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
                baud_counter <= 0;
                
                if (in_serial == 0)
                    rx_state <= STATE_RECEIVE_DATA;
                else
                    rx_state <= STATE_WAIT_FOR_DATA;
            end
        end
        
        // RECEIVING DATA
        else if (rx_state == STATE_RECEIVE_DATA)
        begin
            // count one baud cycle
            if (baud_counter < (BAUD_COUNTER_MAX-1))
            begin
                baud_counter <= baud_counter + 1; 
            end
            
            else
            begin
                baud_counter       <= 0;
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
                baud_counter <= 0;
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
            out_data_valid <= 0;
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
*/