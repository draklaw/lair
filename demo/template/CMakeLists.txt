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

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

include_directories(
)

add_lair_resource(template_resource.cpp ""
	/ ${CMAKE_CURRENT_SOURCE_DIR}/assets
)

add_executable(template
	template_resource.cpp
	main.cpp
	game.cpp
	main_state.cpp
	scene.cpp
	simple_scene.cpp
)

target_link_libraries(template
	lair
)
add_dependencies(demo template)
