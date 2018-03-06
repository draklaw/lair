#!/usr/bin/env python3

#
# Copyright (C) 2015 Simon Boyé
#
# This file is part of lair.
#
# lair is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# lair is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with lair.  If not, see <http://www.gnu.org/licenses/>.
#


from sys import (
	argv,
	stdout,
	stderr,
)
from os import (
	mkdir,
)
from os.path import (
	join,
	exists,
	isdir,
)
from io import (
	StringIO,
)

from jinja2 import (
	Environment,
	FileSystemLoader,
)


include_base      = 'include/lair'
src_base          = 'src'

tmpl_dir          = 'tmpl'
header_tmpl_file  = 'source.h'
source_tmpl_file  = 'source.cpp'
cmake_tmpl_file   = 'CMakeLists.txt'

program_name      = argv.pop(0)
module            = None
class_camel       = None
overwrite         = False
create_module     = False


def usage(*, file=stdout):
	print("Usage: {} [-f] [-h] [-m] MODULE CLASS_NAME".format(program_name), file=file)


def error(msg, *, print_usage=False):
	print('Error:', msg, file=stderr)
	if print_usage:
		usage(file=stderr)
	exit(1)


def camel_case_to_snake_case(camelCase):
	snake = StringIO()
	for i, c in enumerate(camelCase):
		if c.isupper():
			if (    i != 0
			    and c.isupper()
			    and i+1 < len(camelCase)
			    and camelCase[i+1].islower()):
				snake.write('_')
		snake.write(c.lower())
	return snake.getvalue()


if __name__ == '__main__':
	# Sanity check
	if not isdir(tmpl_dir) or not isdir(include_base) or not isdir(src_base):
		error("Some required dir not found. Not running from the base directory ? Aborting.")


	# Argument parsing
	while argv:
		arg = argv.pop(0)
		if arg == '-f':
			overwrite = True
		elif arg == '-h':
			usage()
			exit(0)
		elif arg == '-m':
			create_module = True
		elif arg[0] == '-':
			error("Unrecognized argument \"{}\"".format(arg), print_usage=True)
		else:
			if   module      is None: module      = arg
			elif class_camel is None: class_camel = arg
			else:
				error("Unrecognized argument \"{}\"".format(arg), print_usage=True)

	if module      is None: error("Missing MODULE",     print_usage=True)
	if class_camel is None: error("Missing CLASS_NAME", print_usage=True)


	# Some variables
	class_snake = camel_case_to_snake_case(class_camel)

	print("Module:          ", module)
	print("Class camelCase: ", class_camel)
	print("Class snake_case:", class_snake)

	module_include   = join(include_base, module)
	module_src       = join(src_base,     module)

	print("Module include:  ", module_include)
	print("Module src:      ", module_src)

	module_exists    = exists(module_include) and exists(module_src)

	print("Create module:   ", create_module)

	cmake_file  = join(module_src,     'CMakeLists.txt')
	header_file = join(module_include, class_snake + '.h')
	source_file = join(module_src,     class_snake + '.cpp')

	print("CMake file:      ", cmake_file)
	print("Header file:     ", header_file)
	print("Source file:     ", source_file)


	# Sanity checks
	if not module_exists and not create_module:
		error("Module \"{}\" does not exist".format(module))
	if not overwrite and (
			   exists(header_file)
			or exists(source_file)):
		error("Class \"{}\" seems to already exist. Aborting.".format(class_camel))


	# Templates environment
	env = Environment(loader=FileSystemLoader(tmpl_dir))
	env.globals["module"]      = module
	env.globals["class_camel"] = class_camel
	env.globals["class_snake"] = class_snake

	cmake_tmpl  = env.get_template(cmake_tmpl_file)
	header_tmpl = env.get_template(header_tmpl_file)
	source_tmpl = env.get_template(source_tmpl_file)


	# Module creation
	if create_module:
		print("Creating module \"{}\"...".format(module))
		mkdir(module_src)
		mkdir(module_include)
		with open(cmake_file, "w") as out:
			out.write(cmake_tmpl.render())


	# Source files creation
	with open(header_file, "w") as out:
		out.write(header_tmpl.render())
	with open(source_file, "w") as out:
		out.write(source_tmpl.render())
