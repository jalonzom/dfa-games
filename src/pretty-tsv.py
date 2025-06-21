#!/usr/bin/env python3

import sys

def parse_line(line):
    return line.rstrip("\n").rstrip("\r").split("\t")

header = parse_line(sys.stdin.readline())

for line in sys.stdin:
    line = parse_line(line)
    for (h, v) in zip(header, line):
        print(h, ":", v)
    print('')
