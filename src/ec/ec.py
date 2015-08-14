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

entity_ref_class = AutoClass('EntityRef', is_value = True)

entity_manager_class = AutoClass('EntityManager',
								 [ OPTIONAL_PARAM,
								   (auto_size_t, 'entity_block_size', '1024') ])

(entity_ref_class
 .add_method('isValid', auto_bool)
 .add_method('name', auto_string.const())
 .add_method('parent', entity_ref_class)
 .add_method('firstChild', entity_ref_class)
 .add_method('nextSibling', entity_ref_class)
)

(entity_manager_class
 .add_method('nEntities',             auto_size_t)
 .add_method('nZombieEntities',       auto_size_t)
 .add_method('root',                  entity_ref_class)
 .add_method('entityCapacity',        auto_size_t)
 .add_method('createEntity',          entity_ref_class,
			 [ (entity_ref_class, 'parent'),
	           OPTIONAL_PARAM,
			   (auto_string,      'name', 'NULL')])
 .add_method('destroyEntity',         None,
			 [ (entity_ref_class, 'entity') ])
 .add_method('moveEntity',            None,
			 [ (entity_ref_class, 'entity'),
			   (entity_ref_class, 'new_parent') ])
 .add_method('addSpriteComponent',    None,
			 [ (entity_ref_class, 'entity') ])
 .add_method('removeSpriteComponent', None,
			 [ (entity_ref_class, 'entity') ])
)

ec_module = (
	AutoModule('ec')
		.add_include('../core/core_py.h')
		.add_include('lair/ec/entity.h')
		.add_include('lair/ec/entity_manager.h')
		.add_use_namespace('lair')
		.add_class(entity_ref_class)
		.add_class(entity_manager_class)
)

if __name__ == '__main__':
	if len(argv) == 2:
		base_file = argv[1]
	elif len(argv > 2):
		stderr.write("Usage: {} BASE\n".format(argv[0]))
		exit(1)

	ec_module.write_module(base_file)
