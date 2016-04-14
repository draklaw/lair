#!/usr/bin/env python3

from sys import argv
from os.path import join, dirname, splitext
from subprocess import check_call
from json import dump
from re import compile

_field_re = compile(r'.*=(.*)')
def parse_line(line):
	fields = []
	for tok in line.split():
		m = _field_re.match(tok)
		if(m):
			fields.append(m.group(1))
	return fields

lines = iter(open(argv[1]))

json = {}

line = parse_line(next(lines))
json["name"] = line[0].strip()
json["size"] = int(line[1])

line = parse_line(next(lines))
json["height"] = int(line[0])
json["base"]   = int(line[1])

line = parse_line(next(lines))
json["image"] = line[1][1:-1]

img_file = join(dirname(argv[1]), json["image"])
check_call(["convert", img_file, "PNG32:"+img_file])


line = parse_line(next(lines))
count = int(line[0])

chars = json.setdefault("chars", [])
for i in range(count):
	chars.append(list(map(int, parse_line(next(lines)))))

try:
	line = parse_line(next(lines))
except StopIteration:
	line = [ 0 ]
kern_count = int(line[0])

kern = json.setdefault("kern", [])
for i in range(kern_count):
	kern.append(list(map(int, parse_line(next(lines)))))


out_file = splitext(argv[1])[0] + '.json'
dump(json, open(out_file, 'w'), indent='\t')
