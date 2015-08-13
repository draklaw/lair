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

from io import StringIO

import _lair


LOG_FATAL   = 0
LOG_ERROR   = 1
LOG_WARNING = 2
LOG_LOG     = 3
LOG_INFO    = 4
LOG_DEBUG   = 5

PythonLogger = _lair.core.PythonLogger

MasterLogger = _lair.core.MasterLogger


class Logger(_lair.core.Logger):
	def __init__(self, *args, **kwargs):
		_lair.core.Logger.__init__(self, *args, **kwargs)

	def write(self, level, *args, module_name = None, sep = ''):
		if self.master() is None or level > self.level():
			return
		if module_name is None: module_name = self.default_module_name()
		out = StringIO()
		print(*args, sep=sep, end='', file=out)
		self.master().write(level, module_name, out.getvalue())

	def fatal(self, *args, module_name = None, sep = ''):
		self.write(LOG_FATAL, *args, module_name = module_name, sep = sep)

	def error(self, *args, module_name = None, sep = ''):
		self.write(LOG_ERROR, *args, module_name = module_name, sep = sep)

	def warning(self, *args, module_name = None, sep = ''):
		self.write(LOG_WARNING, *args, module_name = module_name, sep = sep)

	def log(self, *args, module_name = None, sep = ''):
		self.write(LOG_LOG, *args, module_name = module_name, sep = sep)

	def info(self, *args, module_name = None, sep = ''):
		self.write(LOG_INFO, *args, module_name = module_name, sep = sep)

	def debug(self, *args, module_name = None, sep = ''):
		self.write(LOG_DEBUG, *args, module_name = module_name, sep = sep)
