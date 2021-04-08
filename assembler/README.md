# PYRA - Python RISC-V Assembler

I searched online and a lot of other results show up for PYRA. It doesn't bother me at all, in the end this project is only for my personal education.

# Testing
Testing should work by populating the test_programs directory with simple assembly programs with instructions that we support and translating them into machine code with a real working translator and this translator 

Generate tests with GCC from C programs:
`arm-none-eabi-gcc --specs=nosys.specs -o main.out main.c` 
