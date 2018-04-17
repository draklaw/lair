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


#find_package(PythonInterp 3.3)
#find_package(PythonLibs 3.3)

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
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

find_package(Eigen3 REQUIRED)


set(lair_INCLUDE_DIRS
	PUBLIC ${PROJECT_SOURCE_DIR}/include

	PUBLIC ${EIGEN3_INCLUDE_DIR}
	PUBLIC ${SDL2IMAGE_INCLUDE_DIR}
	PUBLIC ${SDL2MIXER_INCLUDE_DIR}
	PUBLIC ${SDL2_INCLUDE_DIR}
)

set(lair_LIBRARIES
	PUBLIC ${SDL2IMAGE_LIBRARY}
	PUBLIC ${SDL2MIXER_LIBRARY}
	PUBLIC ${SDL2_LIBRARY}
)


add_autopy(core/core_py core/core.py)
add_autopy(sys_sdl2/sys_sdl2_py sys_sdl2/sys_sdl2.py)
add_autopy(ec/ec_py ec/ec.py)
add_autopy(lair_py lair.py
	core
	sys_sdl2
	ec
)


add_lair_resource(lair_resource.cpp lair
	shader render_gl3/shader
)


set(lair_SOURCES
	jsoncpp.cpp

	core/lair.cpp
	core/log.cpp
	core/metatype.cpp
	core/path.cpp
	core/text.cpp
	core/parse.cpp
	core/ldl.cpp
	core/property.cpp
	core/json.cpp
	core/image.cpp
	core/signal.cpp
	core/asset_manager.cpp
	core/loader.cpp
	core/bitmap_font.cpp

	fs/abstract_file_system.cpp
	fs/file_system.cpp
	fs/memory_file_system.cpp
	fs/real_file_system.cpp

	geometry/intersection.cpp
	geometry/shape_2d.cpp

	utils/input.cpp
	utils/interp_loop.cpp
	utils/tile_map.cpp
	utils/game_base.cpp
	utils/game_state.cpp

	sys_sdl2/window.cpp
	sys_sdl2/image_loader.cpp
	sys_sdl2/sys_module.cpp
	sys_sdl2/audio_module.cpp

	render_gl3/context.cpp
	render_gl3/orthographic_camera.cpp
	render_gl3/buffer_object.cpp
	render_gl3/vertex_attrib_set.cpp
	render_gl3/vertex_array.cpp
	render_gl3/glsl_source.cpp
	render_gl3/shader_object.cpp
	render_gl3/program_object.cpp
	render_gl3/sampler.cpp
	render_gl3/texture.cpp
	render_gl3/texture_set.cpp
	render_gl3/render_pass.cpp
	render_gl3/renderer.cpp
	render_gl3/render_module.cpp

	ec/entity.cpp
	ec/entity_manager.cpp
	ec/component.cpp
	ec/sprite_renderer.cpp
	ec/sprite_component.cpp
	ec/bitmap_text_component.cpp
	ec/tile_layer_component.cpp
	ec/collision_component.cpp
#	ec/shape_component.cpp

	lair_resource.cpp
)

set(lair_py_SOURCES
	core/python_logger.cpp
	core/core_py.cpp
	sys_sdl2/sys_sdl2_py.cpp
	ec/ec_py.cpp
	lair_py.cpp
)


###


add_library(lair STATIC
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
#	OUTPUT_NAME "_lair"
#	PREFIX ""
)
if(MSVC)
	target_compile_options(lair PUBLIC "/MD")
else()
	target_compile_options(lair PUBLIC "-std=c++11")
endif()


###

#if(PYTHONLIBS_FOUND)
#	add_library(lair_py SHARED
#		${lair_py_SOURCES}
#	)
#	target_include_directories(lair_py
#		${lair_INCLUDE_DIRS}
#		PRIVATE "${PYTHON_INCLUDE_DIRS}"
#	)
#	target_link_libraries(lair_py
#		lair
#		${lair_LIBRARIES}
#	)
#	set_target_properties(lair_py PROPERTIES
#		LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
#		OUTPUT_NAME "_lair"
#		PREFIX ""
#	)
#endif()
