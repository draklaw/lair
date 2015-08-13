#!/usr/bin/env python3

##
##  Copyright (C) 2015 Simon Boyé
##
##  This file is part of lair.
##
##  lair is free software: you can redistribute it and/or modify it
##  under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  lair is distributed in the hope that it will be useful, but
##  WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##  General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with lair.  If not, see <http://www.gnu.org/licenses/>.
##

from sys import path, argv
from os import getcwd
path.append(getcwd())

from autopy import AutoModule


if __name__ == '__main__':
	if len(argv) < 2:
		stderr.write("Usage: {} OUTPUT [MODULE ...]\n".format(argv[0]))
		exit(1)

	base_file = argv[1]

	lair_module = AutoModule('_lair')
	for mod in argv[2:]:
		lair_module.add_include(mod + '/' + mod + '_py.h')
		lair_module.add_submodule(mod)

	lair_module.write_module(base_file)
