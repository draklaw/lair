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


#ifndef LAIR_META_ENUM_INFO_H
#define LAIR_META_ENUM_INFO_H


#include <unordered_map>

#include <lair/core/lair.h>


namespace lair
{


class EnumInfo {
public:
	EnumInfo(const String& name);
	EnumInfo(const EnumInfo& ) = delete;
	EnumInfo(      EnumInfo&&) = delete;
	~EnumInfo() = default;

	EnumInfo& operator=(const EnumInfo& ) = delete;
	EnumInfo& operator=(      EnumInfo&&) = delete;

	EnumInfo& add(int value, const String& label);

	const String& name() const;
	unsigned nFields() const;
	String label(int value) const;
	bool label(String& label, int value) const;
	int value(const String& label, int defaultValue = 0) const;
	bool value(int& value, const String& label) const;

private:
	typedef std::unordered_map<int, String> IntStringMap;
	typedef std::unordered_map<String, int> StringIntMap;

private:
	String       _name;
	IntStringMap _labelFromValue;
	StringIntMap _valueFromLabel;
};


}


#endif
