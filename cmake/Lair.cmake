##
##  Copyright (C) 2018 Simon Boyé
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


find_package(PythonInterp 3)

set(LAIR_RESOURCE_EXECUTABLE "${PROJECT_SOURCE_DIR}/bin/resource.py")

function(add_lair_resource OUT_FILE NAMESPACE)
	if(PYTHONINTERP_FOUND)
		set(DST "${CMAKE_CURRENT_BINARY_DIR}/${OUT_FILE}")

		if(NAMESPACE)
			list(APPEND cmd_args "-n" "${NAMESPACE}")
		endif()

		set(i 2)
		while(i LESS ARGC)
			list(APPEND cmd_args "-r" "${ARGV${i}}")
			math(EXPR i "${i} + 1")
			if(i LESS ARGC)
				list(APPEND cmd_args "${ARGV${i}}")
				list(APPEND deps     "${ARGV${i}}")
			else()
				message(SEND_ERROR "add_lair_ressource requires an even number of arguments")
			endif()
			math(EXPR i "${i} + 1")
		endwhile()

		add_custom_command(
			OUTPUT "${DST}"
			COMMAND "${PYTHON_EXECUTABLE}" "${LAIR_RESOURCE_EXECUTABLE}"
				"-o" "${DST}"
				${cmd_args}
			DEPENDS "${LAIR_RESOURCE_EXECUTABLE}" ${deps}
			COMMENT "Building resource file ${DST}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		)
	endif()
endfunction()
