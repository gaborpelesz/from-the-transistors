# PYAS - Python ARM Assembler

# Testing
Testing should work by populating the test_programs directory with simple assembly programs with instructions that we support and translating them into machine code with a real working translator and this translator 

Generate test cases with ARM assembler:
`arm-none-eabi-as <assembly>.s -o <outfile>`

Generate assembly with GCC from C programs:
`arm-none-eabi-gcc --specs=nosys.specs -o main.out main.c`


