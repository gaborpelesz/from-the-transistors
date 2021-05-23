condition_to_int = {
    "eq" : 0b0000,
    "ne" : 0b0001,
    "cs" : 0b0010, "hs"  : 0b0010,
    "cc" : 0b0011, "lo"  : 0b0011,
    "mi" : 0b0100,
    "pl" : 0b0101,
    "vs" : 0b0110,
    "vc" : 0b0111,
    "hi" : 0b1000,
    "ls" : 0b1001,
    "ge" : 0b1010,
    "lt" : 0b1011,
    "gt" : 0b1100,
    "le" : 0b1101,
    "al" : 0b1110,
    ""   : 0b1110
}



def get_condition_code(op, cond):
    # operations with fix consequences
    if op.lower() in ["cmp", "cmn", "tst", "teq"]:
        S = 0b1
    elif cond and len(cond) == 3 and cond[-1].lower() == 's':
        S = 0b1
        cond = cond[:-1]
    else:
        S = 0b0
    try:
        cond = condition_to_int[cond.lower()]
    except KeyError:
        raise Exception(f"Unsupported/Unimplemented condition code: {cond}")
    
    return cond, S

register_alias = {
    "fp": "r11",
    "ip": "f12",
    "sp": "r13",
    "lr": "r14",
    "pc": "r15"
}

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

shift_opcode_map = {
# shift by    imm,    reg
    'lsl': [0b000, 0b0001],
    'lsr': [0b010, 0b0011],
    'asr': [0b100, 0b0101],
    'ror': [0b110, 0b0111]
}

def register_shift(Rm, shift_operation, shift_operand):
    shift_operation = shift_operation.lower()
    assert shift_operation in shift_opcode_map.keys(), f"Expected a shift operation, but got {shift_operation}"

    # shift with immediate, like: MOV R1, R2, LSL #6
    if shift_operand.startswith('#'):
        shift_imm_5 = shift_operand[1:]
        assert shift_imm_5.isdigit(), f"Immediate must be an integer: '{shift_imm_5}'"
        shift_imm_5 = int(shift_imm_5)
        assert shift_imm_5 >= 0 and shift_imm_5 <= 31, f"Immediate must be 5 bit wide in binary: {shift_imm_5} < {2**5}"

        shift_opcode = shift_opcode_map[shift_operation][0]
        return shift_imm_5 << 7 | shift_opcode << 4 | Rm

    # shift with register, like: MOV R1, R2, LSR R3
    else:
        Rs = reg_to_int(shift_operand)
        shift_opcode = shift_opcode_map[shift_operation][1]
        return Rs << 8 | shift_opcode << 4 | Rm

def immediate(immediate_operands):
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