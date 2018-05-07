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
	VarList();
	VarList(const String& type);
	VarList(const VarList&) = default;
	VarList(VarList&&)      = default;
	~VarList();

	VarList& operator=(const VarList&) = default;
	VarList& operator=(VarList&&)      = default;

	const String& type() const;
	String& type();

protected:
	String _type;
};


}


#endif
