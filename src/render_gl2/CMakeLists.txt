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

set(lair_render_gl2_SOURCES
	gl.cpp
	renderer.cpp
	glsl_source.cpp
	shader_object.cpp
	program_object.cpp
	texture.cpp
)

set(lair_render_gl2_LIBRARIES
)

add_library(lair_render_gl2 ${lair_render_gl2_SOURCES})

target_include_directories(lair_render_gl2
	PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(lair_render_gl2 ${lair_render_gl2_LIBRARIES})
