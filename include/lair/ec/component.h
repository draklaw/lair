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


#ifndef _LAIR_EC_COMPONENT_H
#define _LAIR_EC_COMPONENT_H


#include <lair/core/lair.h>


namespace lair
{


class _Entity;


class Component {
public:
	explicit inline Component(_Entity* entity)
	    : _entityPtr(entity) {
	}
	Component(const Component&) = delete;
	Component(Component&&)      = default;

	Component& operator=(const Component&) = delete;
	Component& operator=(Component&&)      = default;

	inline _Entity* _entity() const { return _entityPtr; }

protected:
	_Entity*  _entityPtr;
};


}


#endif
