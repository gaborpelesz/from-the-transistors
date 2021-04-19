# PYAS - Python ARM Assembler

### TODO maybe move some of this documentation into my CPU's page

## Quick note

This assembler has the only purpose to directly translate the instruction for my ARM7-based CPU. The instruction set is more probably just a subset of the full ISA. It only supports the most basic instruction that are required by the any of the software to run that build on top of this project.

## Running

`pyas <assembly>.s -o <binary_output_name>`

Generate text file with character coded hexadecimals

`pyas <assembly>.s -s <binary_output_name>`

## Instruction set

So far implemented:
- data processing instructions
    - immediates, rotation immediates (e.g., `MOV R1, #63, 28 ; rotate 63 by 28 bits on a 32bit word`)
    - LSL, LSR and other register shifts for data processing instructions (excluding RRX because it is essentially ROR with immediate #0)
    - supported instructions:
        - all data processing instructions (MOV, ADD, AND, CMP, TST, ...)

Registers:
- general purpose registers: R0...R11, except
    - R7: holds syscall number
    - R11: FP - Frame Pointer
- special purpose registers:
    - R12: Intra procedural call
    - R13: Stack pointer
    - R14: Link Register
    - R15: Program counter
    - CPSR: Current program status register

Currently supporting the most basic/common instructions:
- **LDR**: Load word
- **STR**: Store word
- **ADD**: addition
- **SUB**: subtraction
- **MUL**: multiplication
- **LSL**: Left shift
- **LSR**: Right shift
- **CMP**: Compare
- **AND**: Bitwise AND
- **ORR**: Bitwise OR
- **EOR**: Bitwise XOR
- **PUSH**: push on stack
- **POP**: pop off stack
- **B**: Branch
- **BL**: Branch with Link

## Comments

PYAS is using the ARMAS style ';' single line comments.

# Testing
Testing should work by populating the test_programs directory with simple assembly programs with instructions that we support and translating them into machine code with a real working translator and this translator 

Generate test cases with ARM assembler:
`arm-none-eabi-as <assembly>.s -o <outfile>`

Generate assembly with GCC from C programs:
`arm-none-eabi-gcc --specs=nosys.specs -o main.out main.c`