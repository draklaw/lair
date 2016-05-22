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
public:
	explicit Component(ComponentManager* manager, _Entity* entity);
	Component(const Component&) = delete;
	Component(Component&&)      = default;
	virtual ~Component();

	Component& operator=(const Component&) = delete;
	Component& operator=(Component&&)      = default;

	bool isAlive() const { return _alive; }
	ComponentManager* manager() { return _manager; }
	EntityRef entity() { return EntityRef(_entityPtr); }

	void destroy();

	inline _Entity* _entity() const { return _entityPtr; }

public:
	ComponentManager* _manager;
	_Entity*   _entityPtr;
	Component* _nextComponent;

	// FIXME: This should be used only by dense component. Move it ?
	bool _alive;
};


}


#endif
