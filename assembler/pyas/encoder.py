from instruction_formats.data_processing import data_processing_instruction_encode
from instruction_formats.data_processing import data_processing_opcode_map

from instruction_formats.branching import branching_instruction_encode

supported_operations = [
    *data_processing_opcode_map.keys(),
    "b" # branching instructions
]


def instruction_encoder(labeled_instructions):
    binary = []
    
    for i, (labels, instruction) in enumerate(labeled_instructions):
        instruction_address = i*4 # instruction address word aligned (1 word == 4 bytes)

        op_components = instruction.split(" ", 1)
        assert len(op_components) == 2, f"Bad instruction, no operands or no operation was specified: {instruction}"

        # separate operation and operands
        op_cond, operands = op_components
        op_cond = op_cond.strip().lower()
        operands = [operand.strip() for operand in operands.split(",")]

        # find the operation and separate possible condition
        op = list(filter(lambda key: op_cond.startswith(key), supported_operations))
        op = op[0].rstrip()
        cond = op_cond.split(op)[1]

        assert not op is None, f"Unknown operation '{op}' in instruction: {instruction}"

        print(op, cond, operands)
        if op in data_processing_opcode_map.keys():
            assert len(operands) >= 2, f"Operands missing for operation '{op}' in instruction {instruction}"            
            binary.append(data_processing_instruction_encode(op, cond, operands))
        elif op == "b": # branching
            assert len(operands) == 1, f"Unexpected number of operands for operation '{op}' in instruction {instruction}"
            binary.append(branching_instruction_encode(instruction_address, op, cond, operands))
        else:
            raise Exception(f"Unknown operation '{op}' in instruction: {instruction}")

    return binary
