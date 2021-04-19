from instruction_formats.data_processing import data_processing_instruction_encode
from instruction_formats.data_processing import data_processing_opcode_map

supported_operations = [
    *data_processing_opcode_map.keys()
]


def instruction_encoder(labeled_instructions):
    binary = []
    
    for labels, instruction in labeled_instructions:
        op_cond, operands = instruction.split(" ", 1)
        op_cond = op_cond.strip().lower()
        operands = [operand.strip() for operand in operands.split(",")]

        # find operation which might have a condition too
        op = list(filter(lambda key: op_cond.startswith(key), supported_operations))
        assert len(op) == 1, f"Unknown operation '{op_cond}' in instruction: {instruction}"
        op = op[0]
        cond = op_cond.split(op)[1]

        if op in data_processing_opcode_map.keys():
            binary.append(data_processing_instruction_encode(op, cond, operands))
        else:
            raise Exception(f"Unknown operation '{op}' in instruction: {instruction}")


    print([hex(bin_line) for bin_line in binary])