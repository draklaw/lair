#/usr/bin/env python3

from sys import path, argv
from os import getcwd
path.append(getcwd())

from autopy import *

window_class = (
	AutoClass('Window', None)
		.add_getset('width', auto_int)
		.add_getset('height', auto_int)
		.add_getset('title', auto_string, 'utf8Title', 'setUtf8Title')
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
	AutoClass('SysModule')
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
