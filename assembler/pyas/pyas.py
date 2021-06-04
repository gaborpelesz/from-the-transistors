import os
import sys
import argparse

from .preprocess import preprocess_assembly
from .encoder import instruction_encoder

def load_assembly(input_path):
    with open(input_path, "r") as f:
        raw_assembly = f.read()

    return raw_assembly

def word_array_to_bin(words):
    return b''.join([word.to_bytes(4, byteorder='big') for word in words])

def write_binary(dest, words: list):
    # convert all integers into bytes and join them to form a long byte string
    concatenated_bytes = word_array_to_bin(words)
    
    with open(dest, 'wb') as f:
        f.write(concatenated_bytes)

def parse_args(args):
    args = args[1:] # args[0] is the called path

    parser = argparse.ArgumentParser(description="PYAS Assembler. Translate ARM assembly to binary.")

    parser.add_argument("input", type=str, help="A file path to the single assembly file to be binarized.")
    parser.add_argument("-o", "--out", type=str, help="Output file")
    parser.add_argument("-v", "--verilog", action="store_true")

    args = parser.parse_args(args)

    # Check if input file exists and is valid
    assert os.path.isfile(args.input), f"File not found: '{os.path.realpath(args.input)}'"

    # Check if output was specified, if yes, check if file exists
    if args.out is None:
        args.out = args.input + ".o"
    else:
        assert not os.path.exists(args.out), f"File already exists: '{os.path.realpath(args.out)}'"

    return args

def assemble(inpath: str, split_by_words=False):
    raw_assembly = load_assembly(inpath)
    preprocessed_assembly = preprocess_assembly(raw_assembly)
    binary_array = instruction_encoder(preprocessed_assembly)

    if not split_by_words:
        return word_array_to_bin(binary_array)
    
    return binary_array

def _main(args):
    args = parse_args(args)

    raw_assembly = load_assembly(args.input)
    
    preprocessed_assembly = preprocess_assembly(raw_assembly)
    
    binary_array = instruction_encoder(preprocessed_assembly)
    
    if args.verilog:
        print("Verilog generated source:")
        need_space = lambda x :  '  ' if x < 10 else ' ' if x < 100 else ''
        for i, b in enumerate(binary_array):
            byte_index = (i+1)*4
            print(f"{'{'}{need_space(byte_index-1)}mem[{byte_index-1}],",
                    f"{need_space(byte_index-2)}mem[{byte_index-2}],",
                    f"{need_space(byte_index-3)}mem[{byte_index-3}],",
                    f"{need_space(byte_index-4)}mem[{byte_index-4}]{'}'}",
                f"<= 32'b{b:0>32b}; // 0x{b:0>8x} -> {preprocessed_assembly[i][1]}")
    else:
        write_binary(args.out, binary_array)

if __name__ == "__main__":
    _main(sys.argv)