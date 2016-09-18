/*
 *  Copyright (C) 2015 Simon Boyé
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

#include "lair/ec/property.h"


namespace lair
{


void PropertyType::addEnum(int value, const std::string& label) {
	enumMap.push_back(_EnumInfo{value, label});
}

void PropertyType::addFlag(unsigned flag, const std::string& label) {
	flagMap.push_back(_FlagInfo{flag, flag, label});
}

void PropertyType::addFlagSet(unsigned mask, unsigned flags, const std::string& label) {
	flagMap.push_back(_FlagInfo{mask, flags, label});
}

const std::string* PropertyType::findEnum(int value) const {
	for(const _EnumInfo& ei: enumMap) {
		if(value == ei.value)
			return &ei.label;
	}
	return nullptr;
}

const std::string* PropertyType::matchFlag(unsigned flags, unsigned* mask) const {
	for(const _FlagInfo& fi: flagMap) {
		if(flags & fi.mask == fi.flags) {
			if(mask)
				*mask = fi.mask;
			return &fi.label;
		}
	}
	return nullptr;
}




Property::Property(const PropertyType* type, const std::string& name)
	: _type(type)
	, _name(name)
{
}

const PropertyType* Property::type() const {
	return _type;
}

const std::string&  Property::name() const {
	return _name;
}




unsigned PropertyList::nProperties() const {
	return _properties.size();
}

unsigned PropertyList::propertyIndex(const std::string& property) const {
	return _propertyMap.at(property);
}

const Property& PropertyList::property(unsigned index) const {
	return *_properties.at(index);
}


}
