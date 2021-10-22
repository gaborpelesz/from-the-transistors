#!/usr/bin/env python3

import sys
import os
import serial
import time
import argparse

def parse_args(args):
    args = args[1:] # args[0] is the called path

    parser = argparse.ArgumentParser(description="Pinky CPU programmer.")

    parser.add_argument("input", type=str, help="A file path to the single assembly file to program with.")
    parser.add_argument("-B", "--binary", action="store_true", help="If the input is not an assemble file but an assembled binary.")
    parser.add_argument("-p", "--port", type=str, help="Serial port to send the binary to.", default="/dev/ttyUSB1")
    parser.add_argument("-b", "--baud", type=int, help="The baud rate of the FPGA.", default=115200)

    args = parser.parse_args(args)

    # Check if input file exists and is valid
    assert os.path.isfile(args.input), f"File not found: '{os.path.realpath(args.input)}'"

    return args

def main(args):
    args = parse_args(args)

    if args.binary:
        with open(args.input, 'rb') as f:
            bytes_to_send = f.read()
    else:
        bytes_to_send = pyas.assemble(args.input)

    bytes_to_send += (0).to_bytes(4, byteorder='big') # append 4 * 0x00 at the end, which is the "END" signal to the bootrom

    with serial.Serial(args.port, args.baud) as ser:
        for byte in bytes_to_send:
            ser.write(bytes([byte]))
            time.sleep(0.0002) # without sleep sending would collapse because Rx on FPGA can't handle it

    print('FPGA successfully programmed!')

if __name__ == "__main__":
    try:
        import pyas
    except:
        raise ImportError("PYAS assembler is not installed on this system. Before using the Pinky programmer, please install PYAS assembler from: https://github.com/gaborpelesz/from-the-transistors/tree/main/assembler")

    main(sys.argv)
