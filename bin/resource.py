#!/usr/bin/env python3

from sys import stdout

def writeFile(out, logicPath, realPath):
	file = open(realPath, 'rb')

	out.write('\t{\n\t\tstatic const char* data =\n')

	size = 0
	for line in iter(lambda: file.read(16), b''):
		out.write('\t\t\t"')
		size += len(line)
		for b in line:
			out.write('\\x')
			out.write(format(b, '02x'))
		out.write('"\n')
	out.write('\t\t;\n')
	file.close()

	out.write('\t\tfs.addFile(Path("')
	out.write(logicPath)
	out.write('"), ')
	out.write(format(size, 'd'))
	out.write(', data);\n\t}\n')

def writeFiles(out, fnName, files):
	out.write('void ')
	out.write(fnName)
	out.write('(BuiltinFileSystem& fs) {\n')

	for file in files:
		writeFile(out, *file)

	out.write('}\n')


if __name__ == '__main__':
	out = stdout #open('test.cpp', 'w')
	writeFiles(out, 'testFunction', [ ('test.txt', 'foo/test.txt') ])
	out.close()
