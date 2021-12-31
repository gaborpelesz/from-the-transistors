# Pinky - A weaker Thumb on your ARM

Implementation of an ARM7 cpu on an FPGA using Verilog.
- Only a necessary subset of the instructions are available
- Available memory-mapped IO
- Programming through UART (bootrom is burnt to the FPGA's memory)
- Currently without pipelining

## Roadmap
1. create a fully working CPU without pipelining
2. create tests to ensure the correct behavior
3. modify the CPU so it can be pipelining
4. test the CPU until it is not right
