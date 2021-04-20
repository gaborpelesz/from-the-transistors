def validate_label(label):
    # underscore and dot are acceptable
    # special characters in a label
    label = label.replace("_", "")
    label = label.replace(".", "")

    # TODO: check if label is a special word like a register's name

    return label.isalnum()

def label_instruction(lines):
    set_of_labels = set()
    labeled_instructions = [
        [[], ""] # start with one empty instruction with zero labels
    ]

    for i, line in enumerate(lines):
        line = line.strip()
        
        # handle empty line
        if not line:
            continue
        
        components = line.split(":")
        labels = components[:-1]
        instruction = components[-1]

        # assume multiple labels too
        for label in labels:
            label = label.strip()
            
            assert label, f"Empty label at line: {i+1}"
            assert validate_label(label), f"Label '{label}' contains invalid characters at line: {i+1}"
            assert len(set_of_labels.intersection([label])) == 0, f"Label '{label}' already exists. Preprocess error at line: {i+1}"

            # add new label
            set_of_labels.update([label])
            labeled_instructions[-1][0].append(label)

        instruction = instruction.strip()
        if instruction:
            labeled_instructions[-1][1] = instruction 
            labeled_instructions.append([[], ""])

    labeled_instructions = labeled_instructions[:-1] # there is always one empty instruction at the end

    # create instruction addresses for labels
    ordered_labels = list(zip(*labeled_instructions))[0] # transpose and select list of labels
    label_address_map = {}
    for i, instruction_labels in enumerate(ordered_labels):
        for label in instruction_labels:
            label_address_map[label] = i * 4

    # insert address into places where there are labels
    for i in range(len(labeled_instructions)):
        for label in set_of_labels:
            if labeled_instructions[i][1].find(label) != -1:
                labeled_instructions[i][1] = labeled_instructions[i][1].replace(label, f"#{label_address_map[label]}")                

    return labeled_instructions

def preprocess_assembly(raw_assembly):
    """
        return: list of labeled instruction. Each element of the list is a 2-tuple whose first element
                contains a list of labels (which can be an empty list) and the second element contains
                the actual instruction.
    """

    lines = raw_assembly.split("\n")

    # remove single line comments, leading and trailing spaces
    lines = [line.split(";")[0].strip() for line in lines]

    # getting the labels and their corresponding lines on one line if they weren't not
    # insert actual PC address offsets in place of labels
    labeled_instructions = label_instruction(lines)

    return labeled_instructions