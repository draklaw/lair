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


#ifndef LAIR_META_VAR_MAP_H
#define LAIR_META_VAR_MAP_H


#include <unordered_map>

#include <lair/core/lair.h>

#include <lair/meta/variant.h>


namespace lair
{


class VarMap : public std::unordered_map<String, Variant> {
public:
	typedef std::unordered_map<String, Variant> Base;

	using Base::iterator;
	using Base::const_iterator;

	enum Style {
		INLINE = 0x01,
	};

public:
	VarMap(unsigned style = 0);
	VarMap(const String& type, unsigned style = 0);
	VarMap(const VarMap&) = default;
	VarMap(VarMap&&)      = default;
	~VarMap();

	VarMap& operator=(const VarMap&) = default;
	VarMap& operator=(VarMap&&)      = default;

	const Variant& get(const String& id) const;

	const String& type() const;
	String& type();

	unsigned style() const;
	bool isInline() const;

protected:
	String   _type;
	unsigned _style;
};


}


#endif
