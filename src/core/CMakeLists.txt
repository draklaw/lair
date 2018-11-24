##
##  Copyright (C) 2018 Simon Boyé
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


set(lair_core_INCLUDE_DIRS
	PUBLIC ${PROJECT_SOURCE_DIR}/include

	PUBLIC ${EIGEN3_INCLUDE_DIR}
)

set(lair_core_LIBRARIES
)


set(lair_core_SOURCES
	../jsoncpp.cpp

	lair.cpp
	log.cpp
	path.cpp
	text.cpp
	parse.cpp
	signal.cpp
)


###


add_library(lair_core STATIC
	${lair_core_SOURCES}
)
target_include_directories(lair_core
	${lair_core_INCLUDE_DIRS}
)
target_link_libraries(lair_core
	${lair_core_LIBRARIES}
)
set_target_properties(lair_core PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
#	OUTPUT_NAME "_lair"
#	PREFIX ""
)

target_compile_features(lair_core PUBLIC
	cxx_alignas
	cxx_alignof
	cxx_attributes
	cxx_attribute_deprecated
	cxx_auto_type
	cxx_constexpr
	cxx_decltype
	cxx_defaulted_functions
	cxx_defaulted_move_initializers
	cxx_delegating_constructors
	cxx_deleted_functions
	cxx_extern_templates
	cxx_final
	cxx_lambdas
	cxx_noexcept
	cxx_nonstatic_member_init
	cxx_nullptr
	cxx_override
	cxx_range_for
	cxx_right_angle_brackets
	cxx_rvalue_references
	cxx_static_assert
	cxx_strong_enums
	cxx_thread_local
	cxx_trailing_return_types
	cxx_unicode_literals
	cxx_variable_templates
	cxx_variadic_macros
	cxx_variadic_templates
)
