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

#include <lair/meta/property.h>

#include "lair/meta/property_list.h"


namespace lair
{


unsigned PropertyList::nProperties() const {
	return _properties.size();
}

int PropertyList::propertyIndex(const std::string& propertyName) const {
	auto it = _propertyMap.find(propertyName);
	return (it == _propertyMap.end())? -1: it->second;
}

const Property& PropertyList::property(unsigned index) const {
	lairAssert(index < _properties.size());
	return *_properties[index];
}


}
