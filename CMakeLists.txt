##
##  Copyright (C) 2015 Simon Boyé
##
##  This file is part of lair.
##
##  lair is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
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

cmake_minimum_required(VERSION 3.6)

project(lair)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(src)

find_package(Doxygen)

if (DOXYGEN_FOUND)
	set(DOC_NAME    "Lair Engine")
	set(DOC_BRIEF   "Lair Game Engine")
	set(DOC_VERSION "0.227")
	set(DOC_DEST    "${PROJECT_BINARY_DIR}/docs")
	set(DOC_SOURCES "${PROJECT_SOURCE_DIR}/include/lair")
	
	configure_file(Doxyfile.in Doxyfile)
	
	add_custom_target(docs doxygen)
endif()

add_subdirectory(demo EXCLUDE_FROM_ALL)
add_subdirectory(test EXCLUDE_FROM_ALL)
