/*
 *  Copyright (C) 2018 Simon Boyé
 *
 *  This file is part of lair.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include "lair/meta/var_map.h"


namespace lair
{


VarMap::VarMap() {
}


VarMap::VarMap(const String& type)
    : _type(type) {
}


VarMap::~VarMap() {
}


const Variant& VarMap::get(const String& id) const {
	auto it = find(id);
	if(it != end())
		return it->second;
	return Variant::null;
}


const String& VarMap::type() const {
	return _type;
}


String& VarMap::type() {
	return _type;
}


}
