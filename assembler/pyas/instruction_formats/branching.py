from .utils import get_condition_code 

def form_branching_immediate24(instruction_address, immediate_operands):
    assert immediate_operands[0].startswith("#") and len(immediate_operands) == 1
    immediate = immediate_operands[0][1:]

    assert immediate.isdigit()
    target_address = int(immediate)

    base_address = instruction_address + 8

    byte_offset = target_address - base_address

    # assert that byte offset is in range
    assert byte_offset >= -33554432 and byte_offset <= 335554428

    return byte_offset >> 2

def branching_instruction_encode(instruction_address, op, cond, operands):
    # goal: find if operation is B or BL, keep the conditions
    #       like BLE in mind which is not a BL with a cond E
    #       but a B with LE (note: BLLE also exists)
    if cond:
        # if only one condition character, then the operation must be a BL
        if len(cond) == 1:
            assert cond[0] == 'l', f"Unknown operation {op}{cond} in instruction: {op}{cond} {', '.join(operands)}"
            L = 0b1
            cond_code, _ = get_condition_code(op, "")

        # in case of only two conditions characters the operation
        # must be B because all conditions are two char wide
        elif len(cond) == 2:
            L = 0b0
            cond_code, _ = get_condition_code(op, cond)

        # in case of three condition characters the operation must be a BL
        # with additional condition flags to watch for
        elif len(cond) == 3:
            assert cond[0] == 'l', f"Unknown operation {op}{cond} in instruction: {op}{cond} {', '.join(operands)}"
            L = 0b1
            cond_code, _ = get_condition_code(op, cond[1:])
        else:
            raise Exception(f"Unexpected amount of conditions in instruction: {op}{cond} {', '.join(operands)}")
    else:
        L = 0b0
        cond_code, _ = get_condition_code(op, cond[1:])

    try:
        signed_immed_24 = form_branching_immediate24(instruction_address, operands)
    except AssertionError:
        raise Exception(f"Bad branching immediate for instruction: {op}{cond} {', '.join(operands)}")

    return cond_code << 28 | 0b101 << 25 | L << 24 | (signed_immed_24 & 0xff_ff_ff)