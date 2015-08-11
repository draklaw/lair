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


find_package(PythonInterp 3.3 REQUIRED)
find_package(PythonLibs 3.3 REQUIRED)

set(AUTOPY_DIR "${PROJECT_SOURCE_DIR}/tools")

function(add_autopy TARGET MODULE_FILE)
	add_custom_command(
		OUTPUT
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.h"
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.cpp"
		COMMAND "${PYTHON_EXECUTABLE}"
			"${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_FILE}"
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}"
			${ARGN}
		MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_FILE}"
		DEPENDS "${AUTOPY_DIR}/autopy.py" "${AUTOPY_DIR}/autotmpl.py"
		WORKING_DIRECTORY "${AUTOPY_DIR}"
	)
endfunction()


find_package(SDL2 REQUIRED)

set(lair_INCLUDE_DIRS
	PRIVATE "${PROJECT_SOURCE_DIR}/include"
	PRIVATE "${PYTHON_INCLUDE_DIRS}"
	PRIVATE "${SDL2_INCLUDE_DIR}"
)

set(lair_LIBRARIES
	"${SDL2_LIBRARY}"
)

add_autopy(sys_sdl2/sys_sdl2_py sys_sdl2/sys_sdl2.py)
add_autopy(lair_py lair.py sys_sdl2)

set(lair_SOURCES
	core/log.cpp

	utils/input.cpp
	utils/image.cpp

	sys_sdl2/sys_module.cpp
	sys_sdl2/window.cpp
	sys_sdl2/sys_sdl2_py.cpp

	lair_py.cpp
)

#add_subdirectory(core)
#add_subdirectory(utils)
#add_subdirectory(sys_sdl2)
#add_subdirectory(render_gl2)
#add_subdirectory(ec)

add_library(lair SHARED
	${lair_SOURCES}
)
target_include_directories(lair
	${lair_INCLUDE_DIRS}
)
target_link_libraries(lair
	${lair_LIBRARIES}
)
set_target_properties(lair PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
	PREFIX ""
)
