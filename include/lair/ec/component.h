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

#include <lair/ec/entity.h>


namespace lair
{


class _Entity;
class EntityRef;
class ComponentManager;


class Component {
	enum {
		Alive   = 1 << 0,
		Enabled = 1 << 1,
	};

public:
	explicit Component(ComponentManager* manager, _Entity* entity);
	Component(const Component&) = delete;
	Component(Component&&)      = default;
	virtual ~Component();

	Component& operator=(const Component&) = delete;
	Component& operator=(Component&&)      = default;

	inline bool isAlive()   const { return bitsEnabled(_flags, Alive); }
	inline bool isEnabled() const {
		bool enabled = bitsEnabled(_flags, Alive | Enabled);
		lairAssert(!enabled || _entityPtr);
		return enabled && _entityPtr->isEnabled();
	}

	inline void setEnabled(bool enabled) {
		_flags = setBits(_flags, Enabled, enabled);
	}

	inline ComponentManager* manager() { return _manager; }
	inline EntityRef entity() { return EntityRef(_entityPtr); }

	void destroy();

	inline _Entity* _entity() const { return _entityPtr; }

protected:
	ComponentManager* _manager;
	_Entity*          _entityPtr;

public:
	Component*        _nextComponent;
	uint32            _flags;
};


}


#endif
