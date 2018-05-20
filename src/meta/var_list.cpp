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

#include "lair/meta/var_list.h"


namespace lair
{


VarList::VarList(unsigned style)
    : _type()
    , _style(style)
{
}


VarList::VarList(const String& type, unsigned style)
    : _type(type)
    , _style(style)
{
}


VarList::~VarList() {
}


const String& VarList::type() const {
	return _type;
}


String& VarList::type() {
	return _type;
}


unsigned VarList::style() const {
	return _style;
}


bool VarList::isInline() const {
	return _style & INLINE;
}


bool VarList::isCall() const {
	return _style & CALL;
}


}
