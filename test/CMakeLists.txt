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


if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/googletest")
	add_subdirectory(googletest)

	add_custom_target(buildtests)

	include_directories(
		"${PROJECT_SOURCE_DIR}/include"
		${GTEST_INCLUDE_DIRS}
	)

	include_directories(
	)

	add_subdirectory(core)
	add_subdirectory(ldl)
	add_subdirectory(meta)
	#add_subdirectory(utils)
	#add_subdirectory(ec)
endif()
