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


#ifndef LAIR_META_VAR_LIST_H
#define LAIR_META_VAR_LIST_H


#include <vector>

#include <lair/core/lair.h>

#include <lair/meta/variant.h>


namespace lair
{


class Variant;

class VarList : public std::vector<Variant> {
public:
	enum Style {
		INLINE = 0x01,
		CALL   = 0x02,
	};

public:
	VarList(unsigned style = 0);
	VarList(const String& type, unsigned style = 0);
	VarList(const VarList&) = default;
	VarList(VarList&&)      = default;
	~VarList();

	VarList& operator=(const VarList&) = default;
	VarList& operator=(VarList&&)      = default;

	const String& type() const;
	String& type();

	unsigned style() const;
	bool isInline() const;
	bool isCall() const;

protected:
	String   _type;
	unsigned _style;
};


}


#endif
