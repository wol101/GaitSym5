#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re

string_to_comment = 'SOURCE_GROUP'
pattern = re.compile(string_to_comment, re.IGNORECASE | re.MULTILINE)
valid_extentions = ['.cmake']

# get list of files
items = []
for dirpath, dirnames, files in os.walk('.'):
    for name in files:
        if os.path.splitext(name)[1].lower() in valid_extentions:
            items.append(os.path.join(dirpath, name))

print('%d items found' % (len(items)))

for item in items:
    with open(item) as in_file:
        contents = in_file.read()
    if pattern.search(contents) != None:
        print('"%s" contains "%s"' % (item, string_to_comment))
        new_contents = pattern.sub('#' + string_to_comment, contents)
        with open(item, 'w') as out_file:
            print('Rewriting "%s"' % (item))
            out_file.write(new_contents)
