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

#include "lair/meta/enum_info.h"


namespace lair
{


EnumInfo::EnumInfo(const String& name)
    : _name(name)
{}

EnumInfo& EnumInfo::add(int value, const String& label) {
	lairAssert(_labelFromValue.count(value) == 0);
	lairAssert(_valueFromLabel.count(label) == 0);
	_labelFromValue.emplace(value, label);
	_valueFromLabel.emplace(label, value);
	return *this;
}

const String& EnumInfo::name() const {
	return _name;
}

unsigned EnumInfo::nFields() const {
	return _labelFromValue.size();
}

String EnumInfo::label(int value) const {
	auto it = _labelFromValue.find(value);
	return (it == _labelFromValue.end())? String(): it->second;
}

bool EnumInfo::label(String& label, int value) const {
	auto it = _labelFromValue.find(value);
	if(it == _labelFromValue.end()) {
		return false;
	}
	label = it->second;
	return true;
}

int EnumInfo::value(const String& label, int defaultValue) const {
	auto it = _valueFromLabel.find(label);
	return (it == _valueFromLabel.end())? defaultValue: it->second;
}

bool EnumInfo::value(int& value, const String& label) const {
	auto it = _valueFromLabel.find(label);
	if(it == _valueFromLabel.end()) {
		return false;
	}
	value = it->second;
	return true;
}


}
