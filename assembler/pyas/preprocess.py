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

        # prepare for multi label too
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

    return labeled_instructions[:-1] # there is always one empty instruction at the end

def preprocess_assembly(raw_assembly):
    lines = raw_assembly.split("\n")

    # remove single line comments, leading and trailing spaces
    lines = [line.split(";")[0].strip() for line in lines]

    # getting the labels and their corresponding lines on one line if they weren't not
    labeled_instructions = label_instruction(lines)

    # insert actual PC address offsets in place of labels

    return labeled_instructions