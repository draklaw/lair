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

from autopy import *


vector2_class = AutoClass('Vector2', [ OPTIONAL_PARAM,
									   (auto_double, 'x', '0'),
									   (auto_double, 'y', '0')])


log_level_type = AutoType('LogLevel', 'int', 'i', 'PyLong_FromLong', 'PyLong_AsLong', 'int')

#logger_backend_class = AutoClass('LoggerBackend', None)
logger_backend_class = AutoClass('PythonLogger', [ (auto_object, 'file') ])

master_logger_class = AutoClass('MasterLogger')
(master_logger_class
	.add_method('addBackend',    None, [ (logger_backend_class, 'backend') ])
	.add_method('removeBackend', None, [ (logger_backend_class, 'backend') ])
	.add_method('write', None,
		[ (log_level_type, 'level'), (auto_string, 'module_name'), (auto_string, 'message') ])
)

logger_class = AutoClass('Logger',
						 [ (auto_string, 'module_name'), OPTIONAL_PARAM,
						   (master_logger_class, 'master', 'NULL'),
						   (log_level_type, 'log_level', 'LogLevel::Log') ])
(logger_class
	.add_method('master', master_logger_class)
	.add_method('setMaster', None, [ (master_logger_class, 'master') ])
	.add_method('level', log_level_type)
	.add_method('defaultModuleName', auto_string.const())
	.add_method('setDefaultModuleName', None, [ (auto_string.const(), 'module_name') ])
	.add_method('setLevel', None, [ (log_level_type, 'level') ])
	.add_method('setFrom', None, [ (logger_class.value(), 'logger') ], py_id='set_from_logger')
)

core_module = (
	AutoModule('core')
		.add_include("iostream")
		.add_include("lair/core/lair.h")
		.add_include("lair/core/python_logger.h")
		.add_include("lair/core/log.h")
		.add_use_namespace("lair")
		.add_class(logger_backend_class)
		.add_class(master_logger_class)
		.add_class(logger_class)
)

#core_module.add_method('_create_stdout_backend', logger_backend_class,
                       #body_tmpl = Template(
#"""LoggerBackend* backend = new OStreamLogger(std::cout, true);
#return LoggerBackend_FromLoggerBackend(backend);
#"""))

#core_module.add_method('_create_stderr_backend', logger_backend_class,
                       #body_tmpl = Template(
#"""LoggerBackend* backend = new OStreamLogger(std::cerr, true);
#return LoggerBackend_FromLoggerBackend(backend);
#"""))


if __name__ == '__main__':
	if len(argv) == 2:
		base_file = argv[1]
	elif len(argv > 2):
		stderr.write("Usage: {} BASE\n".format(argv[0]))
		exit(1)

	core_module.write_module(base_file)
