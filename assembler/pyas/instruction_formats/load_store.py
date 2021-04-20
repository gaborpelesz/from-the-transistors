from instruction_formats.utils import get_condition_code, \
                                      reg_to_int, \
                                      register_shift, \
                                      shift_opcode_map, \
                                      immediate \

load_store_opcode_map = {
    'ldr': 0b1,
    'str': 0b0
}

def offset_sign(ch):
    # deal with offset sign parameter
    if ch == '-':
        U = 0b0
    elif ch == '+':
        U = 0b1
    else:
        U = 0b1

    return U

def load_store_instruction_encode(op, cond, operands):
    if len(cond) >= 2:
        if cond == 'bt':
            B = 0b1
            cond_code, _ = get_condition_code(op, "")
        else:
            cond_code, _ = get_condition_code(op, cond[:2])
            B = len(cond) > 2 and cond.startswith('b')
    else:
        cond_code, _ = get_condition_code(op, "")
        B = cond != '' and cond.startswith('b')

    L = load_store_opcode_map[op]

    Rd = reg_to_int(operands[0])

    # join the separated operands so we can decide on addressing mode
    addressing_operands = ', '.join(operands[1:])

    # find addressing brackets
    try:
        b_pos = addressing_operands.index('['), addressing_operands.index(']')
        operands_inside_brackets = addressing_operands[b_pos[0] + 1 : b_pos[1]]

        # empty brackets or closing and opening are opposites: ] [
        if operands_inside_brackets == '':
            raise ValueError()

    except ValueError:
        raise Exception(f"Error while encoding '{op}' instruction: {op}{cond} {', '.join(operands)}")


    # Find out type of address mode:
    #   - Three address modes for each offset mode: 
    #       - offset  -> LDR r1, [r2, #13]
    #       - prefix  -> LDR r1, [r2, r3, LSL #28]!
    #       - postfix -> LDR r1, [r2], r3, LSL #30
    #
    # if closing the bracket is the last than this address mode is: offset
    if len(addressing_operands) == b_pos[1] + 1:
        W = 0b0 # not pre-indexed, can be post-indexed or just an offset
        P = 0b1 # not post-indexed

    else:
        ex_split = addressing_operands.split('!')

        # if one '!' was present it is a pre-indexed
        if len(ex_split) == 2:
            # assert that '!' is the last char AND ']' is the last before '!'
            assert ex_split[1].strip() == '' and ex_split[0].rstrip()[-1] == "]", \
                   f"Error while encoding '!' in '{op}' instruction: {op}{cond} {', '.join(operands)}"

            W = 0b1 # is pre-indexed
            P = 0b1 # not post-indexed

        # else it must be post-indexed
        else:
            W = 0b0
            P = 0b0

    # Three offset modes: 
    #   - immediate
    #   - register
    #   - scaled register
    #
    # exclude brackets and exclamation mark and reverse the join, for addressing operand processing
    addressing_operands = addressing_operands.replace('[', '').replace(']', '').replace('!', '').split(', ')

    Rn = reg_to_int(addressing_operands[0])

    if len(addressing_operands) == 1 or len(addressing_operands) == 2:
        # Immediate offset mode
        if len(addressing_operands) == 2 and addressing_operands[1].startswith('#'):
            I = 0b0
            U = offset_sign(addressing_operands[1][1])
            immed_start_i = 0 if addressing_operands[1][1].isdigit() else 1
            addr_mode_12 = immediate([addressing_operands[1][immed_start_i:]])

        # Register offset mode
        else:
            if len(addressing_operands) == 2:
                I = 0b1
                U = offset_sign(addressing_operands[1][0])
                immed_start_i = 0 if addressing_operands[1][0].isdigit() else 1
                addr_mode_12 = reg_to_int(addressing_operands[1][immed_start_i:])

            # if there is only a register with no offset, than it is not an immediate
            else:
                I = 0b0
                U = 0b1
                addr_mode_12 = 0b0

    # Scaled register offset mode
    elif len(addressing_operands) == 3:
        I = 0b1
        U = offset_sign(addressing_operands[1][0])
        immed_start_i = 0 if addressing_operands[1][0].isalpha() else 1

        Rm = reg_to_int(addressing_operands[1][immed_start_i:])
        shift_operation, shift_operands = addressing_operands[2].split(' ')
        addr_mode_12 = register_shift(Rm, shift_operation.lower(), shift_operands)

    else:
        raise Exception(f"Unknown operands for '{op}' operation in instruction: {op}{cond} {', '.join(operands)}")

    print([addressing_operands])
    print(P)
    return cond_code << 28 | 0b1 << 26 | \
           I << 25 | P << 24 | U << 23 | \
           B << 22 | W << 21 | L << 20 | \
           Rn << 16 | Rd << 12 | addr_mode_12


# I = not(is_immediate)
# P = not(is_post_indexed)
# U = is_addition # U=0 -> substraction: [r1, -r2], [r1, #-24]
# B = is_unsigned # LDRB, STRB
# W = is_pre_indexed
# L = is_load
# 
#     I   P   U   B   W   L
# 1.  0   1   U   B   0   L
# 2.  1   1   U   B   0   L
# 3.  1   1   U   B   0   L
# 4.  0   1   U   B   1   L
# 5.  1   1   U   B   1   L
# 6.  1   1   U   B   1   L
# 7.  0   0   U   B   0   L
# 8.  1   0   U   B   0   L
# 9.  1   0   U   B   0   L