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


add_executable(test_core
	test_path.cpp
	test_text.cpp
	test_signal.cpp
	intrusive_pointer/test_base.cpp
	intrusive_pointer/test_foo.cpp
	intrusive_pointer/test_bar.cpp
	intrusive_pointer/test_manager.cpp
	test_intrusive_pointer.cpp
)

target_link_libraries(test_core
	gtest_main
	lair_core
)
add_dependencies(buildtests test_core)
