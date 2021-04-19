data_processing_opcode_map = {
    "and": 0b0000,
    "eor": 0b0001,
    "sub": 0b0010,
    "rsb": 0b0011,
    "add": 0b0100,
    "adc": 0b0101,
    "sbc": 0b0110,
    "rsc": 0b0111,
    "tst": 0b1000,
    "teq": 0b1001,
    "cmp": 0b1010,
    "cmn": 0b1011,
    "orr": 0b1100,
    "mov": 0b1101,
    "bic": 0b1110,
    "mvn": 0b1111
}

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

shift_opcode_map = {
# shift by    imm,    reg
    'lsl': [0b000, 0b0001],
    'lsr': [0b010, 0b0011],
    'asr': [0b100, 0b0101],
    'ror': [0b110, 0b0111]
}

two_operand_ops = [
    'mov',
    'cmp',
    'mvn',
    'cmn',
    'tst',
    'teq'
]

def reg_to_int(reg):
    if not reg[0].lower() == 'r':
        try:
            reg = register_alias[reg.lower()]
        except KeyError:
            raise KeyError(f"Not found alias for register: {reg}")

    assert reg[1:].isdigit(), f"Unknown register: {reg}"
    reg_num = int(reg[1:])
    assert reg_num < 16 and reg_num >= 0, f"Only R0-R15 is available to use"
    return reg_num

def data_processing_register_shift(Rm, shift_operation, shift_operand):
    # shift with immediate, like: MOV R1, R2, LSL #6
    if shift_operand.startswith('#'):
        shift_imm_5 = shift_operand[1:]
        assert shift_imm_5.isdigit(), f"Immediate must be an integer: '{shift_imm_5}'"
        shift_imm_5 = int(shift_imm_5)
        assert shift_imm_5 >= 0 and shift_imm_5 <= 31, f"Immediate must be 5 bit wide in binary: {shift_imm_5} < {2**5}"

        shift_opcode = shift_opcode_map[shift_operation.lower()][0]
        return shift_imm_5 << 7 | shift_opcode << 4 | Rm

    # shift with register, like: MOV R1, R2, LSR R3
    else:
        Rs = reg_to_int(shift_operand)
        shift_opcode = shift_opcode_map[shift_operation.lower()][1]
        return Rs << 8 | shift_opcode << 4 | Rm

def data_processing_immediate32(immediate_operands):
    if len(immediate_operands) == 1:
        immed_12 = immediate_operands[0][1:]

        assert immed_12.isdigit(), f"Immediate must be an integer: '{immed_12}'"
        immed_12 = int(immed_12)
        assert immed_12 <= 0b11_1111_1111 and immed_12 >= 0, f"Immediate must be 12 bit wide so: {immed_12} < {2**12}"
        
        return immed_12

    # check if rotation amount is separate
    if len(immediate_operands) == 2: # e.g., ['#63', '28']
        immed_8 = immediate_operands[0][1:] 
        assert immed_8.isdigit(), f"Immediate must be an integer: '{immed_8}'"
        immed_8 = int(immed_8)
        assert immed_8 <= 0b1111_1111, f"Immediate value must be 8 bit at max: {immed_8} < {2**8}"

        rotate_amount = immediate_operands[1]
        assert rotate_amount.isdigit(), f"Immediate rotate amount must be an integer: '{immed_8}'"
        rotate_amount = int(rotate_amount)
        assert rotate_amount >= 0 and rotate_amount <= 30 and rotate_amount % 2 == 0, f"Immediate rotate amount is equals to 2*rotate_imm_4bit. It must be even and '{immed_8}' <= 30"

        rotate_imm = rotate_amount // 2 
        return rotate_imm << 8 | immed_8 

    else:
        raise AssertionError(f"Unexpected amount of immediate operands: {', '.join(immediate_operands)}")

def get_condition_code(op, cond):
    #if cond and op.lower() in ["cmp"]:
    #    raise Exception(f"Operation '{op}' can't have a condition")

    if op.lower() in ["cmp", "cmn", "tst", "teq"]:
        S = 0b1
    elif cond and cond[-1].lower() == 's':
        S = 0b1
        cond = cond[:-1]
    else:
        S = 0b0
    try:
        cond = condition_to_int[cond.lower()]
    except KeyError:
        raise Exception(f"Unsupported/Unimplemented condition code: {cond}")
    
    return cond, S

def data_processing_instruction_encode(op, cond, operands):
    assert len(operands) >= 2, f"Not enough operands for '{op}' operation in: \'{op} {' '.join(operands)}\'"
    
    # condition code and S specifier
    cond_code, S = get_condition_code(op, cond)

    # operation code
    opcode = data_processing_opcode_map[op]

    # destination register
    Rd = reg_to_int(operands[0])

    # source operands (Rn [, Rm|shift [, shift]])
    # 1. if first source operand is an immediate (like in 'MOV R1, #63')
    if operands[1].startswith("#") and op in two_operand_ops:
        Rn = 0b0000
        if op in ["cmp", "cmn", "tst", "teq"]: # CMP saves the first reg into Rn and not Rd
            Rn = Rd
            Rd = 0b0000
        I = 0b1
        shifter_operand = data_processing_immediate32(operands[1:])

    # 2. if only one source operand but it is not an immediate than it must be a register (like in 'MOV R1, R2')
    elif len(operands) == 2 and op in two_operand_ops:
        Rn = 0b0000
        if op in ["cmp", "cmn", "tst", "teq"]: # CMP saves the first reg into Rn and not Rd
            Rn = Rd
            Rd = 0b0000
        I = 0b0
        shifter_operand = reg_to_int(operands[1])

    else:
    # 3. second source operand is a rotation mnemonic (like in 'MOV R1, R2, LSL #2')
        print(op)
        if len(operands) == 3 and operands[2].split(" ")[0].lower() in shift_opcode_map.keys() and op in two_operand_ops:
            Rn = 0b0000
            if op in ["cmp", "cmn", "tst", "teq"]: # CMP saves the first reg into Rn and not Rd
                Rn = Rd
                Rd = 0b0000
            I = 0b0
            Rm = reg_to_int(operands[1])
            shift_operation, shift_operand = operands[2].split(" ")
            print(Rm, shift_operand.lower())
            shifter_operand = data_processing_register_shift(Rm, shift_operation, shift_operand)

        elif not (op in two_operand_ops) and len(operands) > 2:
    # 4. second source operand is an immediate like in 'ADD R1, R1, #32'
            if not (op in two_operand_ops) and operands[2].startswith("#"):
                Rn = reg_to_int(operands[1])
                I = 0b1
                shifter_operand = data_processing_immediate32(operands[2:])

    # 5. if only two source operands but it is not an immediate than it must be a register
            elif not (op in two_operand_ops) and len(operands) == 3:
                Rn = reg_to_int(operands[1])
                I = 0b0
                shifter_operand = reg_to_int(operands[2])

    # 6. if more than three source operands present than it must be 2 registers with a rotation mnemonic
            elif not (op in two_operand_ops) and len(operands) == 4 and operands[3].split(" ")[0].lower() in shift_opcode_map.keys():
                Rn = reg_to_int(operands[1])
                I = 0b0
                Rm = reg_to_int(operands[2])
                shift_operation, shift_operand = operands[3].split(" ")
                shifter_operand = data_processing_register_shift(Rm, shift_operation, shift_operand)

        else:
            raise Exception(f"Unsupported shifter mnemonic or unexpected operand at: {op}{cond} {', '.join(operands)}")
    
    print(cond_code, I, opcode, S, Rn, Rd, shifter_operand)
    return cond_code << 28 | I << 25 | opcode << 21 | S << 20 | Rn << 16 | Rd << 12 | shifter_operand