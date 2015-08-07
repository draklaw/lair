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

set(lair_ec_SOURCES
	entity.cpp
	entity_manager.cpp
	sprite_component.cpp
)

set(lair_ec_LIBRARIES
)

add_library(lair_ec ${lair_ec_SOURCES})

target_include_directories(lair_ec
	PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(lair_ec ${lair_ec_LIBRARIES})
