#!/usr/bin/env python3

from sys import path, argv
from os import getcwd
path.append(getcwd())

from autopy import AutoModule


if __name__ == '__main__':
	if len(argv) < 2:
		stderr.write("Usage: {} OUTPUT [MODULE ...]\n".format(argv[0]))
		exit(1)

	base_file = argv[1]

	lair_module = AutoModule('lair')
	for mod in argv[2:]:
		lair_module.add_include(mod + '/' + mod + '_py.h')
		lair_module.add_submodule(mod)

	lair_module.write_module(base_file)
