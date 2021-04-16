condition_to_int = {
    "al": 0b1110,
    "": 0b1110
}

register_alias = {
    "fp": "r11",
    "ip": "f12",
    "sp": "r13",
    "lr": "r14",
    "pc": "r15"
}

def reg_to_int(reg):
    print("reg coming:", reg)
    if not reg[0].lower() == 'r':
        try:
            reg = register_alias[reg.lower()]
        except KeyError:
            raise KeyError(f"Not found alias for register: {reg}")

    assert reg[1:].isdigit(), f"Unknown register: {reg}"
    reg_num = int(reg[1:])
    assert reg_num < 16, f"Only R1-R15 is available to use"
    return reg_num

def data_processing_shifter_immediate(value):
    assert value.isdigit(), f"Immediate must be an integer: '{value}'"
    value = int(value)

    assert value <= 0b11_1111_1111, f"Immediate must be 12 bit wide so: {value} < {2**12})"

    return value

def data_processing_instruction_encode(cond, I, opcode, S, Rn, Rd, shifter_operand):
    return cond << 28 | I << 25 | opcode << 21 | S << 20 | Rn << 16 | Rd << 12 | shifter_operand
    

def mov_encode(cond, operands):
    assert len(operands) == 2, f"Wrong operand list for MOV operation: {operands}"
    
    # condition code and S specifier
    if cond and cond[-1].lower() == 's':
        S = 0b1
        cond = cond[:-1]
    else:
        S = 0b0

    try:
        cond = condition_to_int[cond.lower()]
    except KeyError:
        raise Exception(f"Unsupported condition code: {cond}")

    # destination register
    Rd = reg_to_int(operands[0])
    
    # data processing shifter immediate
    if operands[1][0] == '#':
        I = 0b1
        shifter_operand = data_processing_shifter_immediate(operands[1][1:])
    else:
        I = 0b0
        shifter_operand = reg_to_int(operands[1])

    # MOV operation code
    opcode = 0b1101 

    print(cond, I, opcode, S, Rd, shifter_operand)
    return data_processing_instruction_encode(cond, I, opcode, S, 0b0000, Rd, shifter_operand)

operations = {
    "mov": mov_encode
}


def instruction_encoder(labeled_instructions):
    binary = []
    
    for labels, instruction in labeled_instructions:
        op_cond, operands = instruction.split(" ", 1)
        op_cond = op_cond.strip()
        operands = [operand.strip() for operand in operands.split(",")]

        # find operation which might have a condition too
        op = list(filter(lambda key: op_cond.startswith(key), operations.keys()))[0]
        cond = op_cond.split(op)[1]

        binary.append(operations[op](cond, operands))


    print([hex(bin_line) for bin_line in binary])