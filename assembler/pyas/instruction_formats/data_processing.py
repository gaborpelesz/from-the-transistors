from .utils import get_condition_code, \
                                      reg_to_int, \
                                      register_shift, \
                                      shift_opcode_map, \
                                      immediate \


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

two_operand_ops = [
    'mov',
    'cmp',
    'mvn',
    'cmn',
    'tst',
    'teq'
]

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
        shifter_operand = immediate(operands[1:])

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
        if len(operands) == 3 and operands[2].split(" ")[0].lower() in shift_opcode_map.keys() and op in two_operand_ops:
            Rn = 0b0000
            if op in ["cmp", "cmn", "tst", "teq"]: # CMP saves the first reg into Rn and not Rd
                Rn = Rd
                Rd = 0b0000
            I = 0b0
            Rm = reg_to_int(operands[1])
            shift_operation, shift_operand = operands[2].split(" ")
            shifter_operand = register_shift(Rm, shift_operation, shift_operand)

        elif not (op in two_operand_ops) and len(operands) > 2:
    # 4. second source operand is an immediate like in 'ADD R1, R1, #32'
            if not (op in two_operand_ops) and operands[2].startswith("#"):
                Rn = reg_to_int(operands[1])
                I = 0b1
                shifter_operand = immediate(operands[2:])

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
                shifter_operand = register_shift(Rm, shift_operation, shift_operand)

        else:
            raise Exception(f"Unsupported shifter mnemonic or unexpected operand at: {op}{cond} {', '.join(operands)}")
    
    return cond_code << 28 | I << 25 | opcode << 21 | S << 20 | Rn << 16 | Rd << 12 | shifter_operand