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

from os.path import join, dirname
from importlib.machinery import SourceFileLoader
core = SourceFileLoader('core', join(dirname(__file__), '..', 'core', 'core.py')).load_module()

window_class = (
	AutoClass('Window', None)
		.add_getset('width', auto_int)
		.add_getset('height', auto_int)
		.add_getset('title', auto_string.const(), 'utf8Title', 'setUtf8Title')
		.add_method('isValid', auto_bool)
		.add_method('isFullscreen', auto_bool)
		.add_method('isVisible', auto_bool)
		.add_method('resize', None, [ (auto_int, 'width'), (auto_int, 'height') ])
		.add_method('setFullscreen', None, [ (auto_bool, 'fullscreen') ])
		.add_method('setVisible', None, [ (auto_bool, 'visible') ])
		.add_method('destroy')
		.add_method('swapBuffers')
)

sys_module_class = (
	AutoClass('SysModule', [ OPTIONAL_PARAM, (core.master_logger_class, 'MasterLogger', 'NULL') ])
		.add_method('initialize', auto_bool)
		.add_method('shutdown')
		.add_method('isScreensaverEnabled', auto_bool)
		.add_method('setScreensaverEnabled', None, [ (auto_bool, 'enable') ])
		.add_method('isVSyncEnabled', auto_bool)
		.add_method('setVSyncEnabled', None, [ (auto_bool, 'enable') ])
		.add_method('createWindow', window_class, [
		            (auto_string, 'title'), (auto_int, 'width'), (auto_int, 'height') ])
		.add_method('destroyAllWindows')
		.add_method('waitAndDispatchSystemEvents')
		.add_method('dispatchPendingSystemEvents')
		.add_method('getKeyState', auto_int, [ (auto_int, 'scancode') ])
		.add_method('getTimeNs', auto_int64)
		.add_method('waitNs', None, [ (auto_int64, 'ns') ])
)

sys_module_module = (
	AutoModule('sys_sdl2')
		.add_include('../core/core_py.h')
		.add_include('lair/sys_sdl2/sys_module.h')
		.add_include('lair/sys_sdl2/window.h')
		.add_use_namespace('lair')
		.add_class(window_class)
		.add_class(sys_module_class)
)

if __name__ == '__main__':
	if len(argv) == 2:
		base_file = argv[1]
	elif len(argv > 2):
		stderr.write("Usage: {} BASE\n".format(argv[0]))
		exit(1)

	sys_module_module.write_module(base_file)
