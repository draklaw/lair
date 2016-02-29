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

find_package(Eigen3 REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(
	"${EIGEN3_INCLUDE_DIR}"
	"${SDL2_INCLUDE_DIR}"
)

add_executable(simple_window
	simple_window.cpp
)

add_definitions(-D "'SIMPLE_WINDOW_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"'")

target_link_libraries(simple_window
	lair
#	GLEW
#	GL
#	/usr/lib/x86_64-linux-gnu/libGLESv2.so.2
#	${SDL2_LIBRARY}
)
add_dependencies(demo simple_window)
