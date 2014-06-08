import os
import sys
import re

"""
This script fix broken waf configuration file (_cache.py) on Windows.
The configuration file doesn't generate cpp files from ui sources.
Fix only removes the invalid include record.
"""

if len(sys.argv) != 2:
    print("Usage:")
    print("  fixconf.py _conf.py")
    exit(-1)

INPUT_FILENAME = sys.argv[1]
TMP_FILENAME = 'tmp.py'
WRONG = "'C:\\\\Program Files (x86)\\\\Microsoft Visual Studio 12.0\\\\VC\\\\INCLUDE', "

line_pattern = re.compile('^INCLUDES = ')

with open(INPUT_FILENAME, 'r') as input, open(TMP_FILENAME, 'w+') as output:
    for line in input.readlines():
        if line_pattern.match(line):
            line = line.replace(WRONG, "")

        # write line
        output.write(line)

os.replace(TMP_FILENAME, INPUT_FILENAME)
