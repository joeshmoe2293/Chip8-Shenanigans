#!/usr/bin/env python3

import argparse
import re
import sys

def print_raw(prog, print_hex):
    print('Stripped program:')
    if print_hex:
        for instr in prog:
            split_instr = instr[0].split(' ')
            print(f'0x{split_instr[0]} 0x{split_instr[1]}', end=' ')
        print('')
    else:
        print(' '.join([x[0] for x in prog]))

def pretty_print(prog, print_hex):
    print('Program listing:')
    for i, instruction in enumerate(prog):
        if print_hex:
            split_instr = instruction[0].split(' ')
            instruction = (f'0x{split_instr[0]} 0x{split_instr[1]}', instruction[1])
        print(f'{i*2 + 0x200:04X}: {" ".join(instruction)}')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Strip Chip8 ASM from file')
    parser.add_argument('file', type=str, help='ch8 file')
    parser.add_argument('-pp', '--pretty-print', dest='pp', action='store_const', const=True, default=False)
    parser.add_argument('-x', '--hex-print', dest='hex', action='store_const', const=True, default=False)
    args = parser.parse_args()

    with open(args.file, 'r') as f:
        data = f.read()

    out_prog = []
    asm_regex = re.compile(r'^([\dA-Fa-f]{2} ?[\dA-Fa-fxX]{2})(?:\s*(#.*)){0,1}$')
    for line in data.split('\n'):
        match = asm_regex.findall(line)
        if len(match) > 0:
            out_prog.append(match[0])
    
    if args.pp:
        pretty_print(out_prog, args.hex)
    else:
        print_raw(out_prog, args.hex)
