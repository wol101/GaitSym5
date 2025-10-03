#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import argparse
import glob
import re
import math

def apply_genome():
    parser = argparse.ArgumentParser(description='Apply a genome to a GaitSym5 XML config file')
    parser.add_argument('-g', '--genome_file', default='', help='the genome file to use (defaults to last BestGenome*.txt)')
    parser.add_argument('-i', '--input_xml_file', default='workingConfig.xml', help='the input GaitSym XML config file (defaults to workingConfig.xml)')
    parser.add_argument('-o', '--output_xml_file', default='', help='the output GaitSym XML config file (defaults to genome_file name with .xml)')
    parser.add_argument('-l', '--recursion_limit', type=int, default=10000, help='set the python recursion limit (defaults to 10000)')
    parser.add_argument('-r', '--regex_insert', default='{{(.*?)}}', help='the regex to match for the genome replacements (defaults to "{{(.*?)}}")')
    parser.add_argument('-f', '--force', action='store_true', help='force overwrite of destination file')
    parser.add_argument('-v', '--verbose', action='store_true', help='write out more information whilst processing')
    parser.add_argument("-d", "--debug", action="store_true", help="write out more information whilst processing")
    args = parser.parse_args()

    if args.debug:
        args.verbose = True

    # start by creating any missing arguments
    if not args.genome_file:
        files = sorted(glob.glob('BestGenome*.txt'))
        if not files:
            print("genome_file not found: glob('BestGenome*.txt') returned nothing")
            sys.exit(1)
        args.genome_file = files[-1]
    if not args.output_xml_file:
        prefix, suffix = os.path.splitext(args.genome_file)
        args.output_xml_file = prefix + '.xml'

    if args.verbose:
        pretty_print_sys_argv(sys.argv)
        pretty_print_argparse_args(args)

    preflight_read_file(args.genome_file, args.verbose)
    preflight_read_file(args.input_xml_file, args.verbose)
    preflight_write_file(args.output_xml_file, args.force, args.verbose)

    if args.verbose:
        print('Reading gemome file "%s"' % (args.genome_file))
    with open(args.genome_file, 'r') as f:
        lines = f.readlines()
    genome_type = int(lines[0])
    if args.verbose: print('genome_type = %d' % (genome_type))
    num_genes = int(lines[1])
    if args.verbose: print('num_genes = %d' % (num_genes))
    genes = []
    for i in range(0, num_genes):
        l = i + 2
        if l > len(lines):
            print('Not enough genes in "%s"' % (args.genome_file))
            sys.exit(1)
        tokens = lines[l].split()
        if len(tokens) < 1:
            print('Not enough tokens in "%s"' % (args.genome_file))
            sys.exit(1)
        tokens = lines[l].split()
        genes.append(float(tokens[0]))

    if args.verbose: print('Reading input XML file "%s"' % (args.input_xml_file))
    with open(args.input_xml_file, 'r') as f:
        contents = f.read()

    if args.verbose: print('Parsing input XML file "%s"' % (args.input_xml_file))

    pattern = re.compile(args.regex_insert)
    new_contents_parts = []
    insert_index = 0
    expression_count = 0
    for m in re.finditer(pattern, contents):
        expression_count += 1
        if args.debug: print(f'{m.group(1)=} {m.span()=}')
        new_contents_parts.append(contents[insert_index: m.start()])
        insert_index = m.end()
        expression = m.group(1)
        value = eval(expression, {"g": genes})
        if args.debug: print(f'{expression=} {value=}')
        new_contents_parts.append(format(value, '.17g'))
    new_contents_parts.append(contents[insert_index:])
    if args.verbose: print('%d expressions evaluated' % (expression_count))

    new_contents = ''.join(new_contents_parts)
    if args.verbose:
        print('Writing output XML file "%s"' % (args.output_xml_file))
    with open(args.output_xml_file, 'w') as f:
        f.write(new_contents)


def preflight_read_file(filename, verbose):
    if verbose: print('preflight_read_file: "%s"' % (filename))
    if not os.path.exists(filename):
        print("Error: \"%s\" not found" % (filename))
        sys.exit(1)
    if not os.path.isfile(filename):
        print("Error: \"%s\" not a file" % (filename))
        sys.exit(1)

def preflight_write_file(filename, force, verbose):
    if verbose: print('preflight_write_file: "%s"' % (filename))
    if os.path.exists(filename) and not os.path.isfile(filename):
        print("Error: \"%s\" exists and is not a file" % (filename))
        sys.exit(1)
    if os.path.exists(filename) and not force:
        print("Error: \"%s\" exists. Use --force to overwrite" % (filename))
        sys.exit(1)

def preflight_read_folder(folder, verbose):
    if verbose: print('preflight_read_folder: "%s"' % (folder))
    if not os.path.exists(folder):
        print("Error: \"%s\" not found" % (folder))
        sys.exit(1)
    if not os.path.isdir(folder):
        print("Error: \"%s\" not a folder" % (folder))
        sys.exit(1)

def preflight_write_folder(folder, verbose):
    if verbose: print('preflight_write_folder: "%s"' % (folder))
    if os.path.exists(folder):
        if not os.path.isdir(folder):
            print("Error: \"%s\" exists and is not a folder" % (folder))
            sys.exit(1)
    else:
        try:
            os.makedirs(folder, exist_ok = True)
        except OSError as error:
            print(error)
            print('Directory "%s" can not be created' % folder)
            sys.exit(1)

def is_a_number(string):
    """checks to see whether a string is a valid number"""
    if re.match(r'^([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?$', string.strip()) == None:
        return False
    return True

def pretty_print_sys_argv(sys_argv):
    quoted_sys_argv = quoted_if_necessary(sys_argv)
    print((" ".join(quoted_sys_argv)))

def pretty_print_argparse_args(argparse_args):
    for arg in vars(argparse_args):
        print(("%s: %s" % (arg, getattr(argparse_args, arg))))

def quoted_if_necessary(input_list):
    output_list = []
    for item in input_list:
        if re.search(r"[^a-zA-Z0-9_.-]", item): # note inside [] backslash quoting does not work so a minus sign to match must occur last
            item = "\"" + item + "\""
        output_list.append(item)
    return output_list
# program starts here

if __name__ == '__main__':
    apply_genome()
