#!/usr/bin/python
# Reduce number of subsequent empty lines

import os
import re

for path, dirs, files in os.walk('src'):
    for filename in files:
        print("File is " + path + '/' + filename)
        source = path + '/' + filename
        text = open(source).read();
        output = open(source, 'w')
        output.write(re.sub('\n\n\n+', '\n\n', text, re.MULTILINE))
        output.close() 
